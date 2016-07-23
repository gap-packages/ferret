#ifndef TRIGGERING_CDPXQUF
#define TRIGGERING_CDPXQUF

#include <set>
#include "library/library.hpp"
#include "constraints/abstract_constraint.hpp"
#include "triggering_enums.hpp"



struct TraceEvent
{
    TraceEventType event;

    TriggerType trigger_type;
    AbstractConstraint* con;
    vec1<int> con_vec;

    TraceEvent()
    : event(TraceEvent_INVALID), trigger_type(Trigger_INVALID)
    { }

    DEFAULT_MOVE_COPY_CONST_ASSIGN(TraceEvent);

    TraceEvent(TriggerType _type, AbstractConstraint* ac)
    : event(TraceEvent_Constraint), trigger_type(_type), con(ac)
    { D_ASSERT(trigger_type == Trigger_Fix); }

    TraceEvent(TriggerType _type, AbstractConstraint* ac, std::set<int> _s)
    : event(TraceEvent_Constraint), trigger_type(_type), con(ac), con_vec(_s.begin(), _s.end())
    { D_ASSERT(trigger_type == Trigger_Change); }

    TraceEvent(TraceEventType tet)
    : event(tet), trigger_type(Trigger_INVALID), con(NULL)
    { D_ASSERT(tet != TraceEvent_Constraint); }

    SplitState invoke()
    {
        D_ASSERT(event == TraceEvent_Constraint);
        info_out(2, "Executing: " << con->full_name());
        switch(trigger_type)
        {
            case Trigger_Fix:
                return con->signal_fix();
            case Trigger_Change:
                return con->signal_changed(con_vec);
            default:
                abort();
        }
    }

    PartitionStack* getPartitionStack() const
    { return con->getPartitionStack(); }
};

TraceEvent constraint_Fix(AbstractConstraint* ac)
{ return TraceEvent(Trigger_Fix, ac); }

TraceEvent constraint_Change(AbstractConstraint* ac, const std::set<int>& pos)
{ return TraceEvent(Trigger_Change, ac, pos); }


struct BranchEvent
{
    int oldcell;
    int newcell;
    int oldcellsize;
    int newcellsize;

    BranchEvent(int oc, int nc, int ocs, int ncs)
    : oldcell(oc), newcell(nc), oldcellsize(ocs), newcellsize(ncs)
    { }

    friend bool operator==(const BranchEvent& lhs, const BranchEvent& rhs)
    {
        return lhs.oldcell == rhs.oldcell &&
               lhs.newcell == rhs.newcell &&
               lhs.oldcellsize == rhs.oldcellsize &&
               lhs.newcellsize == rhs.newcellsize;
    }

    friend bool operator!=(const BranchEvent& lhs, const BranchEvent& rhs)
    { return !(lhs == rhs); }
};

#endif
