#ifndef _SLOWGRAPH_HPP_AGH
#define _SLOWGRAPH_HPP_AGH

#include <set>

#include "abstract_constraint.hpp"
#include "../partition_stack.hpp"
#include "../partition_refinement.hpp"
#include "library/hash.hpp"

enum GraphDirected
{ GraphDirected_no = 0, GraphDirected_yes = 1};

template<GraphDirected directed = GraphDirected_yes>
class SlowGraph : public AbstractConstraint
{
    vec1<vec1<int> > points;
public:
    virtual std::string name() const
    { return "SlowGraph"; }


    SlowGraph(const vec1<vec1<int> > _points, PartitionStack* ps)
    : AbstractConstraint(ps), points(_points)
    {
        for(int i = 1; i <= points.size(); ++i)
        {
          auto i_size = points[i].size();
          for(int j = 1; j <= i_size; ++j)
          {
            D_ASSERT(points[i][j] <= points.size());
            if(points[i][j] > 0)
              points[points[i][j]].push_back( directed?-i:i );
          }
        }
        for(int i = 1; i <= points.size(); ++i)
        {
          std::set<int> pntset(points[i].begin(), points[i].end());
          points[i] = vec1<int>(pntset.begin(), pntset.end());
        }
        D_ASSERT(points.size() <= ps->domainSize());
    }
private:

    SplitState filterGraph()
    {
        vec1<u_int64_t> mset(ps->domainSize(), 0);
        for(int i = 1; i <= points.size(); ++i)
        {
            //int i_cell = ps->cellOfVal(i);
            for(auto pnt : points[i])
            {
                int pnt_abs = std::abs(pnt);
                int sign = (pnt > 0) ? 1 : -1;

                int it_cell = ps->cellOfVal(pnt_abs);

                mset[i] += quick_hash(it_cell * sign);
            }
        }
        return filterPartitionStackByFunction(ps, ContainerToFunction(&mset));
    }

public:
    SplitState init()
    {
        ps->addTrigger(this, Trigger_Change);
        return filterGraph();
    }

    virtual SplitState signal_changed(const vec1<int>& /*v*/)
    {
        Stats::ConstraintInvoke(Stats::CON_SlowGraph);
        debug_out(1, "slowGraph", "signal_changed");
        return filterGraph();
    }

    virtual bool verifySolution(const Permutation& p)
    {
        for(int i = 1; i <= points.size(); ++i)
        {
            const vec1<int>& p_p_i = points[p[i]];
            vec1<int> image_set;
            for(int j = 1; j <= p_p_i.size(); ++j)
                image_set.push_back(p_p_i[j]);
            std::sort(image_set.begin(), image_set.end());
            if (p_p_i.size() != image_set.size() || !std::equal(p_p_i.begin(), p_p_i.end(), image_set.begin()))
                return false;
        }
        return true;
    }
};

#endif
