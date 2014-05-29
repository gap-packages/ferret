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
        for(int i = 1; i <= points.size(); ++i)
            inv_points[points[i]] = i;
    }

    SplitState init()
    {
        return filterPartitionStackByFunction(ps, ContainerToFunction(&inv_points));
    }

    virtual bool verifySolution(const Permutation& p)
    {
        for(int i = 1; i <= points.size(); ++i)
        {
            if(p[i] != i)
                return false;
        }
        return true;
    }
};

#endif