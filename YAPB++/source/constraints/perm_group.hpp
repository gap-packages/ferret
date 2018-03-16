#ifndef _PERMGROUP_HPP_QWS
#define _PERMGROUP_HPP_QWS

#include <set>

#include "abstract_constraint.hpp"
#include "../partition_stack.hpp"
#include "../partition_refinement.hpp"
#include "../library/algorithms.hpp"
#include "gap_cpp_mapping.hpp"
#include "../rbase/rbase.hpp"

class PermGroup : public AbstractConstraint
{
    Obj group;
    RBase* rb;

public:
    virtual std::string name() const
    { return "Generators_Inefficient"; }

    PermGroup(Obj _group, PartitionStack* ps)
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
      v.push_back(Trigger_RBaseFinished);
      return v;
    }

    SplitState signal_start_buildingRBase()
    {
        return signal_fix_buildingRBase();
    }

    SplitState signal_start()
    {
        return signal_fix();
    }

    virtual void signal_RBaseFinished(RBase* _rb)
    {
        rb = _rb;
    }

    virtual SplitState signal_fix_buildingRBase()
    {
        Stats::ConstraintInvoke(Stats::CON_PermGroup);
        vec1<int> fixed_values;
        const vec1<int>& fixed = ps->fixed_cells();
        for(int i : range1(fixed.size()))
        {
            fixed_values.push_back(*ps->cellStartPtr(fixed[i]));
        }
        vec1<int> part = getRBaseOrbitPartition(fixed_values);
        return filterPartitionStackByFunction(ps, [&](auto i) -> auto& { return part[i]; });
    }

    virtual SplitState signal_fix()
    {
        Stats::ConstraintInvoke(Stats::CON_PermGroup);

        const vec1<int>& new_fix_cells = ps->fixed_cells();

        vec1<int> old_fixed_values;
        vec1<int> new_fixed_values;
        old_fixed_values.reserve(new_fix_cells.size());
        new_fixed_values.reserve(new_fix_cells.size());
        for(int i : range1(new_fix_cells.size()))
        {
            old_fixed_values.push_back(*rb->initial_permstack->cellStartPtr(new_fix_cells[i]));
            new_fixed_values.push_back(*ps->cellStartPtr(new_fix_cells[i]));
        }
        vec1<int> part = getRBaseOrbitPartition(old_fixed_values);
        Obj obj_perm = GAP_callFunction(FunObj_YAPB_RepresentElement, group, GAP_make(old_fixed_values), GAP_make(new_fixed_values));
        debug_out(1, "PermGroup", "mapping from: " << old_fixed_values << " to " << new_fixed_values);
        if(obj_perm == Fail)
        {
            debug_out(1, "PermGroup", "mapping failed");
            return SplitState(false);
        }
        vec1<int> perm = GAP_get<vec1<int> >(obj_perm);
        debug_out(3, "PermGroup", "mapping is: " << perm);
        vec1<int> permuted_part(part.size(), 0);
        for(int i : range1(part.size()))
        {
            if(i <= perm.size())
                permuted_part[perm[i]] = part[i];
            else
                permuted_part[i] = part[i];
        }
        debug_out(3, "PermGroup", "Permuted partition" << permuted_part);
        return filterPartitionStackByFunction(ps, [&](auto i) -> auto& { return permuted_part[i]; });
    }

    virtual bool verifySolution(const Permutation& p)
    { return GAP_get<bool>(GAP_callFunction(FunObj_inGroup, GAP_make(p), group)); }
};

#endif
