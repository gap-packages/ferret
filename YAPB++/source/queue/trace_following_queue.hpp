#ifndef TRACE_FOLLOW_CIUF
#define TRACE_FOLLOW_CIUF
#include "triggering_events.hpp"
#include "abstract_queue.hpp"
#include "library/vec1.hpp"
#include "tracer.hpp"

class TraceFollowingQueue : public AbstractQueue
{
    vec1<TraceList> trace;
    Reverting<int> trace_depth;

    // This is used for tracing individual propagators.
    // We duplicate trace_depth, as updating that for every
    // constraint can get fairly expensive.
    int local_trace_depth;
    int local_trace_split_count;
    int local_trace_sort_count;
public:

    TraceFollowingQueue(const vec1<TraceList>& _trace, MemoryBacktracker* mb)
    : trace(_trace), trace_depth(mb->makeReverting<int>())
    { trace_depth.set(1); }

    void beginBranch()
    {
        local_trace_depth = trace_depth.get();
        local_trace_split_count = 1;
        local_trace_sort_count = 1;
        D_ASSERT(trace[local_trace_depth].traceEvent.event == TraceEvent_Branch);
    }

    void endBranch()
    {
        D_ASSERT(local_trace_split_count == 2);
        D_ASSERT(trace_depth.get() == local_trace_depth);
        trace_depth.set(local_trace_depth + 1);
    }


    SplitState execute_trace()
    {
        debug_out(1, "TraceFollowingQueue", "starting trace");
        int depth = trace_depth.get();

        while(trace[depth].traceEvent.event == TraceEvent_Constraint)
        {
            local_trace_depth = depth;
            local_trace_split_count = 1;
            local_trace_sort_count = 1;
            if(trace[depth].traceEvent.invoke().hasFailed())
            {
                info_out(1,  "trace deviation - invoke failed");
                return SplitState(false);
            }

            if(local_trace_split_count - 1 != trace[depth].branchEvents.size())
            {
                info_out(1, "trace deviation - wrong length");
                return SplitState(false);
            }
            info_out(2, "After splitting: " <<
                        trace[depth].traceEvent.getPartitionStack()->dumpCurrentPartition());

            depth++;
        }
        trace_depth.set(depth);
        debug_out(1, "TraceFollowingQueue", "trace passed");

        return SplitState(true);
    }

    virtual bool hasSortData()
    { return true; }

    virtual PartitionEvent& getPartitionEvent()
    {
        D_ASSERT(local_trace_sort_count <= trace[local_trace_depth].partitionEvents.size());
        return trace[local_trace_depth].partitionEvents[local_trace_sort_count++];
    }

    virtual void addPartitionEvent(PartitionEvent)
    { abort(); }

    virtual SplitState triggerSplit(int oldcell, int newcell, int oldcellsize, int newcellsize)
    {
        BranchEvent thisBranch(oldcell, newcell, oldcellsize, newcellsize);
        if(trace[local_trace_depth].branchEvents.size() < local_trace_split_count)
            return SplitState(false);

        if(trace[local_trace_depth].branchEvents[local_trace_split_count] != thisBranch)
            return SplitState(false);
        local_trace_split_count++;
        return SplitState(true);
    }


    // This should never happen
    virtual void addTrigger(AbstractConstraint*, TriggerType)
    { abort(); }


};
#endif
