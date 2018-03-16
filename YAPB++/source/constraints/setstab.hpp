#ifndef _SETSTAB_HPP_EFR
#define _SETSTAB_HPP_EFR

#include <set>

#include "abstract_constraint.hpp"
#include "../partition_stack.hpp"
#include "../partition_refinement.hpp"
#include <iostream>

class SetStab : public AbstractConstraint
{
    std::set<int> points;
public:

    virtual std::string name() const
    { return "SetStab"; }


    template<typename Container>
    SetStab(const Container& _points, PartitionStack* ps)
    : AbstractConstraint(ps), points(_points.begin(), _points.end())
    { 
    }

    SplitState signal_start()
    {
        debug_out(1, "SetStab", "init " << points.size());
        return filterPartitionStackByFunction(ps,  [this](auto i){ return points.count(i) > 0; });
    }

    virtual bool verifySolution(const Permutation& p)
    {
        for(int x : points)
        {
            if(points.count(p[x]) != 1)
                return false;
        }
        return true;
    }
};

#endif
