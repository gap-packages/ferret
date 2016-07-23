#ifndef _ABSTRACT_CONSTRAINT_HPP_VFJIOVDS
#define _ABSTRACT_CONSTRAINT_HPP_VFJIOVDS
#include <assert.h>
#include "library/vec1.hpp"
#include "split_state.hpp"
#include "../queue/triggering_enums.hpp"

class PartitionStack;
class RBase;
class Permutation;

class AbstractConstraint
{
protected:
    PartitionStack* ps;

    std::string id;
public:

    PartitionStack* getPartitionStack() const
    { return ps; }

    void setId(std::string _id)
    { id = _id; }

    AbstractConstraint(PartitionStack* _ps)
    : ps(_ps)
    { (void)ps; /* warning supression */ }

    // Cell 'newcell' created, of size 1
    virtual SplitState signal_fix()
    { assert(0); }

    // Any change has occurred
    virtual SplitState signal_changed(const vec1<int>& /*v*/)
    { assert(0); }

    // For types which want to distinguish when the rbase is being built
    virtual SplitState signal_fix_buildingRBase()
    { return signal_fix(); }

    // For types which want to distinguish when the rbase is being built
    virtual SplitState signal_changed_buildingRBase(const vec1<int>& v)
    { return signal_changed(v); }

    virtual void signal_RBaseFinished(RBase*)
    { assert(0); }

    virtual ~AbstractConstraint()
    { }

    // return set of required triggers
    virtual std::vector<TriggerType> triggers()
    { return std::vector<TriggerType>(); }

    // perform initial filtering
    virtual SplitState signal_start() = 0;

    virtual SplitState signal_start_buildingRBase()
    { return signal_start(); }

    // Called with solution permutations, to check they are valid.
    virtual bool verifySolution(const Permutation& p) = 0;

    std::string full_name() const {
      return name() + ":" + id;
    }

    // A descriptive name for the constraint
    virtual std::string name() const = 0;

    // Allows constraint to advise which cell to branch on next
    // return -1 for 'no suggestion'
    virtual int advise_branch()
    { return -1; }
};

#endif
