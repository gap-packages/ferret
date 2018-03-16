#ifndef _SETSETSTAB_HPP_PLQZOS
#define _SETSETSTAB_HPP_PLQZOS

#include <set>

#include "abstract_constraint.hpp"
#include "../partition_stack.hpp"
#include "../library/mappers.hpp"
#include "../partition_refinement.hpp"
#include <iostream>
#include <stdexcept>

class SetSetStab : public AbstractConstraint
{
    vec1<std::set<int> > points;
    vec1<int> point_map;
public:

    virtual std::string name() const
    { return "Generators_SetSetStab"; }


    template<typename Container>
    SetSetStab(const Container& _points, PartitionStack* ps)
    : AbstractConstraint(ps), point_map(ps->domainSize(), 0)
    {
        for(const auto& l : _points)
        {
            points.push_back(std::set<int>(l.begin(), l.end()));
        }

        // The reason for this sorting is that it makes implementing 'verifySolution' easier.
        std::sort(points.begin(), points.end());

        for(int i : range1(points.size()))
        {
            for(int val : points[i])
            {
                if(point_map[val] != 0)
                    throw std::runtime_error("Cannot have set of sets with repeated inner points");
                point_map[val] = i;
            }
        }
    }


public:


    virtual SplitState signal_changed(const vec1<int>& /*v*/)
    {
        Stats::ConstraintInvoke(Stats::CON_SetSetStab);
        debug_out(1, "slowGraph", "signal_changed");
        return filterPartitionStackByUnorderedFunction(ps, [&](auto i) -> auto& { return point_map[i]; });
    }

    virtual bool verifySolution(const Permutation& p)
    {
        // We could make this method much faster, but hopefully it gets called so
        // infrequently it doesn't matter.

        vec1<std::set<int> > mapped_points;

        for(const auto& set : points)
        {
            std::set<int> set_image;
            for(const auto& x : set)
            {
                set_image.insert(p[x]);
            }
            mapped_points.push_back(set_image);
        }

        std::sort(mapped_points.begin(), mapped_points.end());
        return points == mapped_points;
    }


    std::vector<TriggerType> triggers()
    {
      std::vector<TriggerType> v;
      v.push_back(Trigger_Change);
      return v;
    }

    SplitState signal_start()
    {
        debug_out(1, "SetSetStab", "init " << points.size());
        std::set<int> all_values;
        for(int i : range1(points.size()))
            all_values.insert(points[i].begin(), points[i].end());
        SplitState ss = filterPartitionStackByFunction(ps,  [&all_values](auto i){ return all_values.count(i) > 0; });
        if(ss.hasFailed())
          return ss;
        return filterPartitionStackByUnorderedFunction(ps, [&](auto i) -> auto& { return point_map[i]; });
    }
};

#endif
