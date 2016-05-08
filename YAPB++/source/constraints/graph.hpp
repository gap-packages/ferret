#ifndef _SLOWGRAPH_HPP_AGH
#define _SLOWGRAPH_HPP_AGH

#include <set>
#include <string.h>

#include "abstract_constraint.hpp"
#include "../partition_stack.hpp"
#include "../partition_refinement.hpp"
#include "library/hash.hpp"
#include "library/mono_set.hpp"
#include "library/graph.hpp"

template<GraphDirected directed = GraphDirected_yes>
class SlowGraph : public AbstractConstraint
{
    vec1<vec1<int> > points;
    
public:
    virtual std::string name() const
    { return "SlowGraph"; }


    SlowGraph(const vec1<vec1<int> >& _points, PartitionStack* ps)
    : AbstractConstraint(ps), points(_points),
    advise_branch_monoset(ps->domainSize())
    {
        for(int i = 1; i <= _points.size(); ++i)
        {
            int i_size = _points[i].size();
            for(int j = 1; j <= i_size; ++j)
            {
                D_ASSERT(_points[i][j] > 0);
                points[_points[i][j]].push_back(directed?-i:i);
            }
        }
        for(int i = 1; i <= points.size(); ++i)
        {
            std::set<int> pntset(points[i].begin(), points[i].end());
            points[i] = vec1<int>(pntset.begin(), pntset.end());
        }
        D_ASSERT(points.size() <= ps->domainSize());
        mset.resize(ps->domainSize(), 0);
    }
private:

    // Construct this once, for use in filterGraph, as the cost is fairly big
    vec1<u_int64_t> mset;
    
    SplitState filterGraph(const vec1<int>& cells)
    {
        // Would not normally go this low level, but it is important this is fast
        memset(&(mset.front()), 0, mset.size() * sizeof(mset[0]));
        
        MonoSet monoset(ps->cellCount());
        debug_out(0, "SlowGraph", "filtering: " << cells.size() << " cells out of " << ps->cellCount());
        int nodes = 0, edges = 0;
        for(int c = 1; c <= cells.size(); ++c)
        {
            Range<PartitionStack::cellit> r = ps->cellRange(cells[c]);
            for(PartitionStack::cellit it = r.begin(); it != r.end(); ++it)
            {
                int i = *it;
                nodes++;
                int i_cell = ps->cellOfVal(i);
                int poshash = quick_hash(i_cell);
                int neghash = quick_hash(-i_cell);
                for(vec1<int>::iterator it = points[i].begin(); it != points[i].end(); ++it)
                {
                    edges++;
                    int valabs = std::abs(*it);
                    bool valsign = (*it) > 0;

                    if(valsign)
                        mset[valabs] += poshash;
                    else
                        mset[valabs] += neghash;
                    monoset.add(ps->cellOfVal(valabs));
                }
            }
        }
        debug_out(0, "SlowGraph", "considered " << nodes << " nodes and " << edges << " edges.");
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
        Stats::ConstraintInvoke(Stats::CON_SlowGraph);
        debug_out(1, "slowGraph", "signal_changed");
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
                for(vec1<int>::iterator edge = points[cellfirstmem].begin();
                                        edge != points[cellfirstmem].end(); ++edge)
                {
                    int cell = ps->cellOfVal(std::abs(*edge));
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
            const vec1<int>& p_i = points[i];
            vec1<int> image_set;
            for(int j = 1; j <= p_i.size(); ++j) {
                int pnt = p_i[j];
                int pnt_image = pnt<0?-p[-pnt]:p[pnt];
                image_set.push_back(pnt_image);
            }
            std::sort(image_set.begin(), image_set.end());
            
            if(points[p[i]] != image_set)
              return false;
        }
        return true;
    }
};

#endif
