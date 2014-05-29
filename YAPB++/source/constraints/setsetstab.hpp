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
        typedef typename Container::const_iterator in_con;
        for(in_con it = _points.begin(); it != _points.end(); ++it)
        {
            points.push_back(std::set<int>(it->begin(), it->end()));
        }

        // The reason for this sorting is that it makes implementing 'verifySolution' easier.
        std::sort(points.begin(), points.end());

        for(int i = 1; i <= points.size(); ++i)
        {
            for(std::set<int>::const_iterator it = points[i].begin(); it != points[i].end(); ++it)
            {
                if(point_map[*it] != 0)
                    throw std::runtime_error("Cannot have set of sets with repeated inner points");
                point_map[*it] = i;
            }
        }
    }

    
public:


    virtual SplitState signal_changed(const vec1<int>& /*v*/)
    {
        debug_out(1, "slowGraph", "signal_changed");
        return filterPartitionStackByUnorderedFunction(ps, ContainerToFunction(&point_map)); 
    }

    virtual bool verifySolution(const Permutation& p)
    {
        // We could make this method much faster, but hopefully it gets called so 
        // infrequently it doesn't matter.

        vec1<std::set<int> > mapped_points;
        
        typedef std::set<int>::iterator it_type;
        for(int i = 1; i <= points.size(); ++i)
        {
            std::set<int> set_image;
            for(it_type it = points[i].begin(); it != points[i].end(); ++it)
            {
                set_image.insert(p[*it]);
            }
            mapped_points.push_back(set_image);
        }

        std::sort(mapped_points.begin(), mapped_points.end());
        return points == mapped_points;
    }


    SplitState init()
    {
        ps->addTrigger(this, Trigger_Change);
        debug_out(1, "SetSetStab", "init " << points.size());
        std::set<int> all_values;
        for(int i = 1; i <= points.size(); ++i)
            all_values.insert(points[i].begin(), points[i].end());
        SplitState ss = filterPartitionStackByFunction(ps, InSet(&all_values));
        (void)ss;
        D_ASSERT(!ss.hasFailed());
        return filterPartitionStackByUnorderedFunction(ps, ContainerToFunction(&point_map));
    }
};

#endif