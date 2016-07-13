#ifndef _TRACER_HPP_CPRLIXUQ
#define _TRACER_HPP_CPRLIXUQ

#include "library/vec1.hpp"
#include "triggering_events.hpp"
#include "memory_backtrack.hpp"
#include <utility>
#include "abstract_queue.hpp"

struct TraceList
{
    TraceEvent traceEvent;
    vec1<BranchEvent> branchEvents;
    vec1<PartitionEvent> partitionEvents;

    TraceList() {};
    DEFAULT_MOVE_COPY_CONST_ASSIGN(TraceList);

    TraceList(TraceEvent te) : traceEvent(te)
    { }
};


class TracerGenerator : public BacktrackableType
{
    RevertingStack<TraceList> trace;
public:

    TracerGenerator(MemoryBacktracker* mb) :
    BacktrackableType(mb),
    trace(mb->makeRevertingStack<TraceList>())
    {
        trace.push_back(TraceList());
    }

    void beginConstraint(TraceEvent id)
    {
        trace.push_back(TraceList(id));
    }

    void endConstraint()
    {

    }

    void clearTrace()
    { trace.clearStack_dangerous(); }

    virtual void event_pushWorld()
    {
        trace.push_back(TraceList(TraceEvent(TraceEvent_Branch)));
    }

    void addEvent(BranchEvent be)
    {
        trace.getMutable_dangerous().back().branchEvents.push_back(std::move(be));
    }

    void addPartitionEvent(PartitionEvent se)
    {
        trace.getMutable_dangerous().back().partitionEvents.push_back(std::move(se));
    }

    const vec1<TraceList>& getTrace() const
    { return trace.get(); }

};



#endif
