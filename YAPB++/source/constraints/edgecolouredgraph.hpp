#ifndef _EDGEGRAPH_FDJDJQPQ
#define _EDGEGRAPH_FDJDJQPQ

#include <set>
#include <string.h>

#include "abstract_constraint.hpp"
#include "../partition_stack.hpp"
#include "../partition_refinement.hpp"
#include "library/hash.hpp"
#include "library/mono_set.hpp"
#include "library/graph.hpp"

template<typename VertexType, GraphDirected directed = GraphDirected_yes>
class EdgeColouredGraph : public AbstractConstraint
{
    vec1<vec1<VertexType> > points;

public:
    virtual std::string name() const
    { return "Graph<" + VertexType::type() + ">"; }

    
    EdgeColouredGraph(const vec1<vec1<VertexType> >& _points, PartitionStack* ps)
    : AbstractConstraint(ps), points(_points),
    advise_branch_monoset(ps->domainSize())
    {
        D_ASSERT(points.size() <= ps->domainSize());
        points.resize(ps->domainSize());
        mset.resize(ps->domainSize(), 0);
        for(int i = 1; i <= _points.size(); ++i)
        {
            int i_size = _points[i].size();
            for(int j = 1; j <= i_size; ++j)
            {
                if(_points[i][j].target() <= 0 || _points[i][j].target() > ps->domainSize()) {
                    throw GAPException(name() + " contains out-of-bounds vertex: " + toString(_points[i][j].target()));
                }
                
                if(_points[i][j].colour() < 0 ) {
                    throw GAPException(name() + " contains invalid edge colour: " + toString(_points[i][j].colour()));
                }
                VertexType edge(i, _points[i][j].colour());
                if(directed)
                {
                  edge = edge.flipped();
                }

                points[_points[i][j].target()].push_back(edge);
            }
        }
        for(int i = 1; i <= points.size(); ++i)
        {
            std::set<VertexType> pntset(points[i].begin(), points[i].end());
            points[i] = vec1<VertexType>(pntset.begin(), pntset.end());
        }
    }
private:

    // Construct this once, for use in filterGraph, as the cost is fairly big
    vec1<u_int64_t> mset;
    
    SplitState filterGraph(const vec1<int>& cells)
    {
        // Would not normally go this low level, but it is important this is fast
        memset(&(mset.front()), 0, mset.size() * sizeof(mset[0]));
        
        MonoSet monoset(ps->cellCount());
        debug_out(0, "EdgeGraph", "filtering: " << cells.size() << " cells out of " << ps->cellCount());
        int nodes = 0, edges = 0;
        for(int c = 1; c <= cells.size(); ++c)
        {
            Range<PartitionStack::cellit> r = ps->cellRange(cells[c]);
            for(PartitionStack::cellit it = r.begin(); it != r.end(); ++it)
            {
                int i = *it;
                nodes++;
                int i_cell = ps->cellOfVal(i);
                int hash = quick_hash(i_cell);
                for(typename vec1<VertexType>::iterator it = points[i].begin(); it != points[i].end(); ++it)
                {
                    edges++;
                    // TODO: Is this sufficient?
                    mset[it->target()] += quick_hash(hash + it->colour());
                    monoset.add(ps->cellOfVal(it->target()));
                }
            }
        }
        debug_out(0, "EdgeGraph", "considered " << nodes << " nodes and " << edges << " edges.");
        return filterPartitionStackByFunctionWithCells(ps, ContainerToFunction(&mset), monoset);
    }

public:

    std::vector<TriggerType> triggers()
    {
      std::vector<TriggerType> v;
      v.push_back(Trigger_Change);
      return v;
    }

    SplitState signal_start()
    {
        vec1<int> cells;
        for(int i = 1; i <= ps->cellCount(); ++i)
            cells.push_back(i);
        return filterGraph(cells);
    }

    virtual SplitState signal_changed(const vec1<int>& v)
    {
        Stats::ConstraintInvoke(Stats::CON_EdgeGraph);
        debug_out(1, "EdgeGraph", "signal_changed");
        /*
        vec1<int> cells;
        for(int i = 1; i <= ps->cellCount(); ++i)
            cells.push_back(i);
        SplitState ss = filterGraph(cells);
        if(ss.hasFailed())
            return ss;
        return filterGraph(cells);*/
        return filterGraph(v);
    }

    virtual void debug_check_propagation()
    {
        int cellcount = ps->cellCount();
        vec1<int> cells;
        for(int i = 1; i <= cellcount; ++i)
            cells.push_back(i);
        SplitState ss = filterGraph(cells);
        (void)ss;
        D_ASSERT(!ss.hasFailed());
        D_ASSERT(cellcount == ps->cellCount());
    }

    // We cache this monoset to save allocations.
    MonoSet advise_branch_monoset;
    virtual int advise_branch()
    {
        int best_cell = -1;
        int best_neighbours = 0;
        int best_size = std::numeric_limits<int>::max();
        for(int i = 1; i <= ps->cellCount(); ++i)
        {
            if(ps->cellSize(i) > 1)
            {
                advise_branch_monoset.clear();
                int cellfirstmem = *(ps->cellStartPtr(i));
                for(typename vec1<VertexType>::iterator edge = points[cellfirstmem].begin();
                                                        edge != points[cellfirstmem].end(); ++edge)
                {
                    int cell = ps->cellOfVal(edge->target());
                    if(ps->cellSize(cell) > 1)
                        advise_branch_monoset.add(cell);
                }

                if(advise_branch_monoset.size() > best_neighbours ||
                   (advise_branch_monoset.size() == best_neighbours &&
                    ps->cellSize(i) < best_size)
                  )
                {
                    best_cell = i;
                    best_neighbours = advise_branch_monoset.size();
                    best_size = ps->cellSize(i);
                }
            }
        }

        return best_cell;
    }

  virtual bool verifySolution(const Permutation& p)
    {
        for(int i = 1; i <= points.size(); ++i)
        {
            const vec1<VertexType>& p_i = points[i];
            vec1<VertexType> image_set;
            for(int j = 1; j <= p_i.size(); ++j) {
                int pnt = p_i[j].target();
                int pnt_image = p[pnt];
                image_set.push_back(VertexType(pnt_image, p_i[j].colour()));
            }
            std::sort(image_set.begin(), image_set.end());
            
            if(points[p[i]] != image_set) {
              return false;
            }
        }
        return true;
    }
};

#endif
