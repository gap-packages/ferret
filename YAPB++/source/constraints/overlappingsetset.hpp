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

        for(int i : range1(points.size()))
        {
            for(const auto& p : points[i])
            {
                if(!point_map[p].empty() && point_map[p].back() == i)
                    throw std::runtime_error("Inner sets cannot have repeated values");
                point_map[p].push_back(i);
            }
        }
    }


public:


    virtual SplitState signal_changed(const vec1<int>& /*v*/)
    {
        Stats::ConstraintInvoke(Stats::CON_OverlappingSetSetStab);

        debug_out(1, "slowGraph", "signal_changed");
        return filterPartitionStackByUnorderedListFunction(ps, [&](auto i) -> auto& { return point_map[i]; });
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
      for(int i : range1(points.size()))
          all_values.insert(points[i].begin(), points[i].end());
      SplitState ss = filterPartitionStackByFunction(ps, [&all_values](auto i){ return all_values.count(i) > 0; });
      if(ss.hasFailed())
        return ss;
      return filterPartitionStackByUnorderedListFunction(ps, [&](auto i) -> auto& { return point_map[i]; });
    }

    virtual std::vector<TriggerType> triggers()
    {
      std::vector<TriggerType> v;
      v.push_back(Trigger_Change);
      return v;
    }

};

#endif
