#ifndef _NORMALISER_HPP
#define _NORMALISER_HPP

#include <set>

#include "abstract_constraint.hpp"
#include "../partition_stack.hpp"
#include "../partition_refinement.hpp"
#include "../library/algorithms.hpp"
#include "gap_cpp_mapping.hpp"
#include "../rbase/rbase.hpp"

/* Attempt at implementing normalisers in partition backtrack
 * At the moment this just implements Heiko Theißens algorithm
 *
 * It should be sufficient to just compute the normaliser of a group
 * E <= S_n, then we can intersect E with another group G and get
 * the normaliser of E in G.
 *
 * Use with extreme portions of salt, this isn't finished.
 */

class Normaliser : public AbstractConstraint
{
    Obj group;


public:
    virtual std::string name() const
    { return "Normaliser"; }

    Normaliser(Obj _group, PartitionStack* ps)
        : AbstractConstraint(ps), group(_group)
    {
    }
private:

vec1<int> getRBaseOrbitPartition(const vec1<int>& fix)
    {
        debug_out(3, "PermGroup", "Fixing: " << fix);
        Obj vec = GAP_make(fix);
        Obj orbits = GAP_callFunction(FunObj_YAPB_FixedOrbits, group, GAP_make(ps->domainSize()), vec);
        vec1<vec1<int> > oart = GAP_get<vec1<vec1<int> > >(orbits);
        debug_out(3, "PermGroup", "Got orbit partition" << oart);
        // This might not be necessary, but it doesn't hurt!
        for(int i : range1(oart.size()))
            std::sort(oart[i].begin(), oart[i].end());
        std::sort(oart.begin(), oart.end());
        vec1<int> filter = partitionToList(oart, ps->domainSize(), MissingPoints_Fixed);
        debug_out(3, "PermGroup", "Filter partition: " << filter);
        return filter;
    }

public:
    std::vector<TriggerType> triggers()
    {
        std::vector<TriggerType> v;
         v.push_back(Trigger_Fix);
        return v;
    }

    SplitState signal_start()
    {
        debug_out(0, "normaliser", "starting");
        return signal_fix();
    }

 virtual SplitState signal_fix()
    {
        Stats::ConstraintInvoke(Stats::CON_PermGroup);
        vec1<int> fixed_values;
        const vec1<int>& fixed = ps->fixed_cells();
        for(int i : range1(fixed.size()))
        {
            fixed_values.push_back(*ps->cellStartPtr(fixed[i]));
        }
        vec1<int> part = getRBaseOrbitPartition(fixed_values);
        return filterPartitionStackByUnorderedFunction(ps, SquareBrackToFunction(&part));
    }
    
    virtual bool verifySolution(const Permutation& p)
    {
        debug_out(0,"normaliser", "verifying");
        /* we could in principle first test whether p \in group, not
           sure whether that's worth it, I should test that */
        return GAP_get<bool>(GAP_callFunction(FunObj_isGroupConj, GAP_make(p), group));
    }
};


#endif // _NORMALISER_HPP

