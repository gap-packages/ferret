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

  // Construct these once, for use in filterGraph, as the cost is fairly big
    vec1<HashType> mset;
    vec1<HashType> msetspare;
    
    int edgesconsidered;
    
    template<typename GraphType>
    void hashCellSimple(PartitionStack* ps, const GraphType& points, MonoSet& monoset, int cell)
    {
        Range<PartitionStack::cellit> r = ps->cellRange(cell);
        for(int i : r)
        {
            debug_out(4, "filter", "filtering " << i);
            int i_cell = ps->cellOfVal(i);
            int hash = quick_hash(i_cell);
            debug_out(4, "filter", "initial cell" << ps->cellOfVal(i) << ":" << i_cell);
            for(const auto& edge : points.neighbours(i))
            {
                monoset.add(ps->cellOfVal(edge.target()));
                HashType new_hash = quick_hash(hash + edge.colour());
                debug_out(4, "filter", "adding to " << edge << ":" << new_hash);
                edgesconsidered++;
                mset[edge.target()] += new_hash;
            }
        }
    }
    
    template<typename GraphType>
    void hashNeighboursOfVertexDeep2(PartitionStack* ps, const GraphType& points, 
                                     MonoSet& hitcells, int vertex, HashType hash)
    {
        for(const auto& edge : points.neighbours(vertex))
        {
            hitcells.add(ps->cellOfVal(edge.target()));
            HashType new_hash = quick_hash(hash + edge.colour());
            edgesconsidered++;
            msetspare[edge.target()] += new_hash;
        }
    }
 
    template<typename Range, typename GraphType>
    void hashRangeDeep2(PartitionStack* ps, const GraphType& points, MonoSet& hitcells, Range cell)
    {
        for(int i : cell)
        {
            int i_cell = ps->cellOfVal(i);
            int hash = quick_hash(i_cell + mset[i]);
            hashNeighboursOfVertexDeep2(ps, points, hitcells, i, hash);
        }
    }
    
    template<typename GraphType>
    void hashNeighboursOfVertexDeep(PartitionStack* ps, const GraphType& points, 
                                    MonoSet& hitcells, MonoSet& hitvertices, int vertex, HashType hash)
    {
        for(const auto& val : points.neighbours(vertex))
        {
            hitcells.add(ps->cellOfVal(val.target()));
            hitvertices.add(val.target());
            HashType new_hash = quick_hash(hash + val.colour());
            edgesconsidered++;
            mset[val.target()] += new_hash;
        }
    }
 
    template<typename Range, typename GraphType>
    void hashRangeDeep(PartitionStack* ps, const GraphType& points, 
                       MonoSet& hitcells, MonoSet& hitvertices, Range cell)
    {
        for(int i : cell)
        {
            int i_cell = ps->cellOfVal(i);
            int hash = quick_hash(i_cell);
            hashNeighboursOfVertexDeep(ps, points, hitcells, hitvertices, i, hash);
        }
    }
    
    template<typename GraphType, typename CellList>
    SplitState filterGraph(PartitionStack* ps, const GraphType& points,
                           const CellList& cells, int path_length)
    {
        timing_start("GraphRefining");
        // Would not normally go this low level, but it is important this is fast
        memset(&(mset.front()), 0, mset.size() * sizeof(mset[0]));
        edgesconsidered = 0;
        MonoSet monoset(ps->cellCount());
        debug_out(3, "EdgeGraph", "filtering: " << cells.size() << " cells out of " << ps->cellCount());
        if(path_length == 1) {
            for(int c : cells)
            {
                hashCellSimple(ps, points, monoset, c);
            }
        }
        else
        {
            MonoSet hitvertices(ps->domainSize());
            for(int c : cells)
            {
                hashRangeDeep(ps, points, monoset, hitvertices, ps->cellRange(c));
            }
            
            memset(&(msetspare.front()), 0, msetspare.size() * sizeof(msetspare[0]));
            hashRangeDeep2(ps, points, monoset, hitvertices.getMembers());
            for(int i : range1(mset.size())) {
                mset[i] += msetspare[i] * 71;
            }
        }
        debug_out(3, "EdgeGraph", "filtering " << mset << " : " << monoset);
        auto ret = filterPartitionStackByFunctionWithCells(ps, [&](auto i) -> auto& { return mset[i]; }, monoset);
        timing_end("GraphRefining");
        return ret;
    }
};

template<typename VertexType, GraphDirected directed = GraphDirected_yes>
class EdgeColouredGraph : public AbstractConstraint
{
    Graph<VertexType, directed> points;
    GraphConfig config;

    GraphRefiner refiner;

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
        return refiner.filterGraph(ps, points, range1(ps->cellCount()), config.start_path_length);
    }

    virtual SplitState signal_changed(const vec1<int>& v)
    {
        Stats::ConstraintInvoke(Stats::CON_EdgeGraph);
        debug_out(1, "EdgeGraph", "signal_changed");
        return refiner.filterGraph(ps, points, v, config.normal_path_length);
    }

    // We cache this monoset to save allocations.
    MonoSet advise_branch_monoset;
    virtual int advise_branch()
    {
        int best_cell = -1;
        int best_neighbours = 0;
        int best_size = std::numeric_limits<int>::max();
        for(int i : range1(ps->cellCount()))
        {
            if(ps->cellSize(i) > 1)
            {
                advise_branch_monoset.clear();
                int cellfirstmem = *(ps->cellStartPtr(i));
                for(const auto& edge : points.neighbours(cellfirstmem))
                {
                    int cell = ps->cellOfVal(edge.target());
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
        for(int i : range1(points.vertices()))
        {
            const vec1<VertexType>& p_i = points.neighbours(i);
            vec1<VertexType> image_set;
            for(const auto& edge : p_i) {
                int pnt = edge.target();
                int pnt_image = p[pnt];
                image_set.push_back(VertexType(pnt_image, edge.colour()));
            }
            std::sort(image_set.begin(), image_set.end());
            
            if(points.neighbours(p[i]) != image_set) {
              return false;
            }
        }
        return true;
    }
};

#endif
