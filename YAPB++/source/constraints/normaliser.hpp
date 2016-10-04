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
    RBase* rb;

public:
    virtual std::string name() const
    { return "Normaliser"; }

    Normaliser(Obj _group, PartitionStack* ps)
        : AbstractConstraint(ps), group(_group)
    {
    }
private:
    vec1<int> getRBasePartition(const vec1<int>& fix)
    {
    }
public:
    std::vector<TriggerType> triggers()
    {
        std::vector<TriggerType> v;
        v.push_back(Trigger_RBaseFinished);
        return v;
    }
    SplitState signal_start_buildingRBase()
    {
        debug_out(0, "normaliser", "starting rbase");
    }
    SplitState signal_start()
    {
        debug_out(0, "normaliser", "starting");
    }
    virtual void signal_RBaseFinished(RBase* _rb)
    {
        rb = _rb;
    }
    // default of this is signal_fix (because some things don't care)
    virtual SplitState signal_fix_buildingRBase(int)
    {
        debug_out(0, "normaliser", "fix while building rbase");
    }
    virtual SplitState signal_fix(int)
    {
        debug_out(0, "normaliser", "fix");
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
