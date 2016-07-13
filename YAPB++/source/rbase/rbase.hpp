#ifndef _RBASE_HPP_YPLSW
#define _RBASE_HPP_YPLSW

#include "library/library.hpp"
#include "library/vec1.hpp"
#include "memory_backtrack.hpp"
#include "partition_stack.hpp"
#include "queue/tracer.hpp"
#include "library/shared_ptr.hpp"

class RBase
{
	// Make non-copyable
	RBase(const RBase&);
	void operator=(const RBase&);
public:

	RBase() : initial_permstack(0)
	{ }
	
    vec1<int> branchcell;
    vec1<int> branchvalue;
    PartitionStack* initial_permstack;
    vec1<TraceList> trace;
    vec1<int> value_ordering;
    vec1<int> inv_value_ordering;

    int depth() const
    { return branchcell.size(); }

    ~RBase()
    { delete initial_permstack; }
};


#endif
