#ifndef _SETTUPLESTAB_HPP_KDSAA
#define _SETTUPLESTAB_HPP_KDSAA

#include <set>

#include "abstract_constraint.hpp"
#include "../partition_stack.hpp"
#include "../library/mappers.hpp"
#include "../partition_refinement.hpp"
#include <iostream>
#include <stdexcept>

class SetTupleStab : public AbstractConstraint
{
    vec1<vec1<int> > points;
    // Inner elements are of the form <set, tuplepos>
    vec1<vec1<std::pair<int, int> > > point_map;
public:

    virtual std::string name() const
    { return "SetTupleStab"; }

    template<typename Container>
    SetTupleStab(const Container& _points, PartitionStack* ps)
    : AbstractConstraint(ps), point_map(ps->domainSize(), 0)
    {
        for(const auto& line : _points)
        {
            points.push_back(vec1<int>(line.begin(), line.end()));
        }

        // The reason for this sorting is that it makes implementing 'verifySolution' easier.
        std::sort(points.begin(), points.end());

        for(int i : range1(points.size()))
        {
            for(const auto& pos : range1(points[i].size()))
            {
                auto p = points[i][pos];
                point_map[p].push_back(std::make_pair(i, pos));
            }
        }
    }


public:


    virtual SplitState signal_changed(const vec1<int>& /*v*/)
    {
        Stats::ConstraintInvoke(Stats::CON_OverlappingSetSetStab);

        debug_out(1, "slowGraph", "signal_changed");
        return filterPartitionStackBySetTupleFunction(ps, [&](auto i) -> auto& { return point_map[i]; });
    }

    virtual bool verifySolution(const Permutation& p)
    {
        // We could make this method much faster, but hopefully it gets called so
        // infrequently it doesn't matter.

        vec1<vec1<int> > mapped_points;

        for(const auto& tuple : points)
        {
            vec1<int> tuple_image;
            for(int x : tuple)
            {
                tuple_image.push_back(p[x]);
            }
            mapped_points.push_back(tuple_image);
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
      return filterPartitionStackBySetTupleFunction(ps, [&](auto i) -> auto& { return point_map[i]; });
    }

    virtual std::vector<TriggerType> triggers()
    {
      std::vector<TriggerType> v;
      v.push_back(Trigger_Change);
      return v;
    }

};

#endif
