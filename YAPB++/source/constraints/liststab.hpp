#ifndef _LISTSTAB_HPP_AFEQO
#define _LISTSTAB_HPP_AFEQO

#include "abstract_constraint.hpp"
#include "../partition_stack.hpp"
#include "../partition_refinement.hpp"

class ListStab : public AbstractConstraint
{
    vec1<int> points;
    vec1<int> inv_points;
public:

    virtual std::string name() const
    { return "ListStab"; }

    ListStab(const vec1<int>& _points, PartitionStack* ps)
    : AbstractConstraint(ps), points(_points), inv_points(ps->domainSize(), 0)
    {
        for(int i : range1(points.size()))
            inv_points[points[i]] = i;
    }

    SplitState signal_start()
    {
        return filterPartitionStackByFunction(ps, [&](auto i) -> auto& { return inv_points[i]; });
    }

    virtual bool verifySolution(const Permutation& p)
    {
        for(int i : range1(points.size()))
        {
            if(p[points[i]] != points[i])
                return false;
        }
        return true;
    }
};

#endif
