#ifndef _OVERLAP_SETSETSTAB_HPP_KDSAA
#define _OVERLAP_SETSETSTAB_HPP_KDSAA

#include <set>

#include "abstract_constraint.hpp"
#include "../partition_stack.hpp"
#include "../library/mappers.hpp"
#include "../partition_refinement.hpp"
#include <iostream>
#include <stdexcept>

class OverlapSetSetStab : public AbstractConstraint
{
    vec1<std::set<int> > points;
    vec1<vec1<int> > point_map;
public:

    virtual std::string name() const
    { return "OverlapSetSetStab"; }

    template<typename Container>
    OverlapSetSetStab(const Container& _points, PartitionStack* ps)
    : AbstractConstraint(ps), point_map(ps->domainSize(), 0)
    {
        for(const auto& line : _points)
        {
            points.push_back(std::set<int>(line.begin(), line.end()));
        }

        // The reason for this sorting is that it makes implementing 'verifySolution' easier.
        std::sort(points.begin(), points.end());

        for(int i = 1; i <= points.size(); ++i)
        {
            for(std::set<int>::const_iterator it = points[i].begin(); it != points[i].end(); ++it)
            {
                if(!point_map[*it].empty() && point_map[*it].back() == i)
                    throw std::runtime_error("Inner sets cannot have repeated values");
                point_map[*it].push_back(i);
            }
        }
    }


public:


    virtual SplitState signal_changed(const vec1<int>& /*v*/)
    {
        Stats::ConstraintInvoke(Stats::CON_OverlappingSetSetStab);

        debug_out(1, "slowGraph", "signal_changed");
        return filterPartitionStackByUnorderedListFunction(ps, ContainerToFunction(&point_map));
    }

    virtual bool verifySolution(const Permutation& p)
    {
        // We could make this method much faster, but hopefully it gets called so
        // infrequently it doesn't matter.

        vec1<std::set<int> > mapped_points;

        for(const auto& set : points)
        {
            std::set<int> set_image;
            for(int x : set)
            {
                set_image.insert(p[x]);
            }
            mapped_points.push_back(set_image);
        }

        std::sort(mapped_points.begin(), mapped_points.end());
        return points == mapped_points;
    }


    virtual SplitState signal_start()
    {
      debug_out(1, "OverlappingSetSetStab", "init " << points.size());
      std::set<int> all_values;
      for(int i = 1; i <= points.size(); ++i)
          all_values.insert(points[i].begin(), points[i].end());
      SplitState ss = filterPartitionStackByFunction(ps, InSet(&all_values));
      if(ss.hasFailed())
        return ss;
      return filterPartitionStackByUnorderedListFunction(ps, ContainerToFunction(&point_map));
    }

    virtual std::vector<TriggerType> triggers()
    {
      std::vector<TriggerType> v;
      v.push_back(Trigger_Change);
      return v;
    }

};

#endif
