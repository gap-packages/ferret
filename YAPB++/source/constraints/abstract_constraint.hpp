#ifndef _ABSTRACT_CONSTRAINT_HPP_VFJIOVDS
#define _ABSTRACT_CONSTRAINT_HPP_VFJIOVDS
#include <assert.h>
#include "library/vec1.hpp"
#include "split_state.hpp"

class PartitionStack;
class RBase;
class Permutation;

class AbstractConstraint
{
protected:
    PartitionStack* ps;
public:
    AbstractConstraint(PartitionStack* _ps)
    : ps(_ps) 
    { (void)ps; /* warning supression */ }
    
    // Cell 'newcell' created, of size 1
    virtual SplitState signal_fix(int /*firstfix*/)
    { assert(0); }

    // Any change has occurred
    virtual SplitState signal_changed(const vec1<int>& /*v*/)
    { assert(0); }

    // For types which want to distinguish when the rbase is being built
    virtual SplitState signal_fix_buildingRBase(int i)
    { return signal_fix(i); }

    // For types which want to distinguish when the rbase is being built
    virtual SplitState signal_changed_buildingRBase(const vec1<int>& v)
    { return signal_changed(v); }

    virtual void signal_RBaseFinished(RBase*)
    { assert(0); }

    virtual ~AbstractConstraint()
    { }

    // Called once, at start of search
    // Note: Partition may already be split
    virtual SplitState init() = 0;

    // Called with solution permutations, to check they are valid.
    virtual bool verifySolution(const Permutation& p) = 0;

    // A descriptive name for the constraint
    virtual std::string name() const = 0;
};

#endif
