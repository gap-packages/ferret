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


template<typename VertexType, GraphDirected directed>
class GraphRefiner
{

    GraphRefiner();
public:
    GraphRefiner(int points) :
     mset(points, 0),
     msetspare(points, 0),
     edgesconsidered(0)
    {

    };

    typedef Graph<VertexType, directed> GraphType;

  // Construct these once, for use in filterGraph, as the cost is fairly big
    vec1<u_int64_t> mset;
    vec1<u_int64_t> msetspare;
    
    int edgesconsidered;
    
    void hashCellSimple(PartitionStack* ps, const GraphType& points, MonoSet& monoset, int cell)
    {
        Range<PartitionStack::cellit> r = ps->cellRange(cell);
        for(PartitionStack::cellit it = r.begin(); it != r.end(); ++it)
        {
            int i = *it;
            int i_cell = ps->cellOfVal(i);
            int hash = quick_hash(i_cell);
            for(typename vec1<VertexType>::const_iterator it2 = points.edges[i].begin();
              it2 != points.edges[i].end(); ++it2)
            {
                monoset.add(ps->cellOfVal(it2->target()));
                u_int64_t new_hash = quick_hash(hash + it2->colour());
                edgesconsidered++;
                mset[it2->target()] += new_hash;
            }
        }
    }
    
    void hashNeighboursOfVertexDeep2(PartitionStack* ps, const GraphType& points, 
                                     MonoSet& hitcells, int vertex, u_int64_t hash)
    {
        for(typename vec1<VertexType>::const_iterator it = points.edges[vertex].begin();
              it != points.edges[vertex].end(); ++it)
        {
            hitcells.add(ps->cellOfVal(it->target()));
            u_int64_t new_hash = quick_hash(hash + it->colour());
            edgesconsidered++;
            msetspare[it->target()] += new_hash;
        }
    }
 
    template<typename Range>
    void hashRangeDeep2(PartitionStack* ps, const GraphType& points, MonoSet& hitcells, Range cell)
    {
        for(typename Range::iterator it = cell.begin(); it != cell.end(); ++it)
        {
            int i = *it;
            int i_cell = ps->cellOfVal(i);
            int hash = quick_hash(i_cell + mset[i]);
            hashNeighboursOfVertexDeep2(ps, points, hitcells, i, hash);
        }
    }
    
    void hashNeighboursOfVertexDeep(PartitionStack* ps, const GraphType& points, 
                                    MonoSet& hitcells, MonoSet& hitvertices, int vertex, u_int64_t hash)
    {
        for(typename vec1<VertexType>::const_iterator it = points.edges[vertex].begin();
              it != points.edges[vertex].end(); ++it)
        {
            hitcells.add(ps->cellOfVal(it->target()));
            hitvertices.add(it->target());
            u_int64_t new_hash = quick_hash(hash + it->colour());
            edgesconsidered++;
            mset[it->target()] += new_hash;
        }
    }
 
    template<typename Range>
    void hashRangeDeep(PartitionStack* ps, const GraphType& points, 
                       MonoSet& hitcells, MonoSet& hitvertices, Range cell)
    {
        for(typename Range::iterator it = cell.begin(); it != cell.end(); ++it)
        {
            int i = *it;
            int i_cell = ps->cellOfVal(i);
            int hash = quick_hash(i_cell);
            hashNeighboursOfVertexDeep(ps, points, hitcells, hitvertices, i, hash);
        }
    }
    
    SplitState filterGraph(PartitionStack* ps, const GraphType& points,
                           const vec1<int>& cells, int path_length)
    {
        // Would not normally go this low level, but it is important this is fast
        memset(&(mset.front()), 0, mset.size() * sizeof(mset[0]));
        edgesconsidered = 0;
        MonoSet monoset(ps->cellCount());
        debug_out(3, "EdgeGraph", "filtering: " << cells.size() << " cells out of " << ps->cellCount());
        if(path_length == 1) {
            for(int c = 1; c <= cells.size(); ++c)
            {
                hashCellSimple(ps, points, monoset, cells[c]);
            }
        }
        else
        {
            MonoSet hitvertices(ps->domainSize());
            for(int c = 1; c <= cells.size(); ++c)
            {
                hashRangeDeep(ps, points, monoset, hitvertices, ps->cellRange(cells[c]));
            }
            
            memset(&(msetspare.front()), 0, msetspare.size() * sizeof(msetspare[0]));
            hashRangeDeep2(ps, points, monoset, hitvertices.getMembers());
            for(int i = 1; i <= mset.size(); ++i) {
                mset[i] += msetspare[i] * 71;
            }
        }
        return filterPartitionStackByFunctionWithCells(ps, ContainerToFunction(&mset), monoset);
    }
};

template<typename VertexType, GraphDirected directed = GraphDirected_yes>
class EdgeColouredGraph : public AbstractConstraint
{
    Graph<VertexType, directed> points;
    GraphConfig config;

    GraphRefiner<VertexType, directed> refiner;

public:
    virtual std::string name() const
    { return "Graph<" + VertexType::type() + ">"; }

    
    EdgeColouredGraph(const vec1<vec1<VertexType> >& _points, GraphConfig gc, PartitionStack* ps)
    : AbstractConstraint(ps), points(_points, ps->domainSize()), config(gc),
    refiner(ps->domainSize()),
    advise_branch_monoset(ps->domainSize())
    { }
private:

  
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
        return refiner.filterGraph(ps, points, cells, config.start_path_length);
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
        return refiner.filterGraph(ps, points, v, config.normal_path_length);
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
                for(typename vec1<VertexType>::iterator edge = points.edges[cellfirstmem].begin();
                                                        edge != points.edges[cellfirstmem].end(); ++edge)
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
        for(int i = 1; i <= points.edges.size(); ++i)
        {
            const vec1<VertexType>& p_i = points.edges[i];
            vec1<VertexType> image_set;
            for(int j = 1; j <= p_i.size(); ++j) {
                int pnt = p_i[j].target();
                int pnt_image = p[pnt];
                image_set.push_back(VertexType(pnt_image, p_i[j].colour()));
            }
            std::sort(image_set.begin(), image_set.end());
            
            if(points.edges[p[i]] != image_set) {
              return false;
            }
        }
        return true;
    }
};

#endif
