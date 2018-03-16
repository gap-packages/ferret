#ifndef _FIXALLPOINTS_HPP_EKAKA
#define _FIXALLPOINTS_HPP_EKAKA

#include <set>

#include "abstract_constraint.hpp"
#include "../partition_stack.hpp"
#include "../partition_refinement.hpp"
#include <iostream>

// This constraint is useless (it just fixes all points)
// it represents the identity group, and is useful because
// the identity group has a stupid stabilizer chain.
class FixAllPoints : public AbstractConstraint
{
public:

    virtual std::string name() const
    { return "FixAllPoints"; }


    FixAllPoints(PartitionStack* ps)
    : AbstractConstraint(ps)
    {
    }

    SplitState signal_start()
    {
        debug_out(1, "FixAllPoints", "init");
        vec1<int> points;
        for(int i : range1(ps->domainSize()))
            points.push_back(i);
        return filterPartitionStackByFunction(ps, [&](auto i) -> auto& { return points[i]; });
    }

    virtual bool verifySolution(const Permutation& p)
    {
        for(int i : range1(p.size()))
            if(p[i] != i)
                return false;
        return true;
    }
};

#endif
