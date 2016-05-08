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

// Store an edge-coloured graph.
// We require users only give non-negative colours,
// we use negative colours to represent reversed direction edges
class ColEdge {
  int tar;
  int col;
public:
  ColEdge(int _target, int _colour)
  : tar(_target),
    col(_colour)
  { D_ASSERT(col >= 0); D_ASSERT(tar > 0); }

  ColEdge(std::pair<int, int> p)
  : tar(p.first),
    col(p.second)
  { D_ASSERT(col >= 0); D_ASSERT(tar > 0); }

  // we want to allow colour = 0
  ColEdge flipped() const
  {
    ColEdge c(tar, 0);
    c.col = -1 - col;
    return c;
  }

  int target() const { return tar; }
  int colour() const { return col; }

  friend bool operator==(const ColEdge& lhs, const ColEdge& rhs)
  { return lhs.tar == rhs.tar && lhs.col == rhs.col; }

  friend bool operator<(const ColEdge& lhs, const ColEdge& rhs)
  {
    if(lhs.tar < rhs.tar) return true;
    if(lhs.tar > rhs.tar) return false;
    if(lhs.col < rhs.col) return true;
    if(lhs.col > rhs.col) return false;
    return true;
  }
};

template<GraphDirected directed = GraphDirected_yes>
class EdgeColouredGraph : public AbstractConstraint
{
    vec1<vec1<ColEdge> > points;

public:
    virtual std::string name() const
    { return "EdgeColouredGraph"; }

    EdgeColouredGraph(const vec1<vec1<ColEdge> >& _points, PartitionStack* ps)
    : AbstractConstraint(ps), points(_points),
    advise_branch_monoset(ps->domainSize())
    {
        for(int i = 1; i <= points.size(); ++i)
        {
            int i_size = points[i].size();
            for(int j = 1; j <= i_size; ++j)
            {
                ColEdge edge(i, points[i][j].colour());
                if(directed)
                {
                  edge = edge.flipped();
                }

                points[points[i][j].target()].push_back(edge);
            }
        }
        for(int i = 1; i <= points.size(); ++i)
        {
            std::set<ColEdge> pntset(points[i].begin(), points[i].end());
            points[i] = vec1<ColEdge>(pntset.begin(), pntset.end());
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
                for(vec1<ColEdge>::iterator it = points[i].begin(); it != points[i].end(); ++it)
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
                for(vec1<ColEdge>::iterator edge = points[cellfirstmem].begin();
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
    {/*
        for(int i = 1; i <= points.size(); ++i)
        {
            const vec1<int>& p_p_i = points[p[i]];
            vec1<int> image_set;
            for(int j = 1; j <= p_p_i.size(); ++j)
                image_set.push_back(p_p_i[j]);
            std::sort(image_set.begin(), image_set.end());
            if (p_p_i.size() != image_set.size() || !std::equal(p_p_i.begin(), p_p_i.end(), image_set.begin()))
                return false;
        }*/
        return true;
    }
};

#endif
