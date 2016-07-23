#ifndef _QUEUE_CIOGLQICV
#define _QUEUE_CIOGLQICV

#include "triggering_events.hpp"
#include "constraints/abstract_constraint.hpp"
#include "tracer.hpp"
#include "abstract_queue.hpp"
#include "../rbase/rbase.hpp"
#include <set>



class ConstraintQueue : public AbstractQueue
{
    std::vector<AbstractConstraint*> constraint_fix_list;
    std::vector<AbstractConstraint*> constraint_change_list;

    std::vector<AbstractConstraint*> constraint_rbase_finished_list;

    std::vector<int> to_invoke_fix;
    std::vector<std::set<int> > to_invoke_change;

    TracerGenerator* tracer;
public:
    ConstraintQueue(TracerGenerator* _tracer)
    : tracer(_tracer)
    { }

    void addTrigger(AbstractConstraint* ac, TriggerType tt)
    {
        switch(tt)
        {
            case Trigger_Fix:
            {
                D_ASSERT(std::find(constraint_fix_list.begin(),
                                   constraint_fix_list.end(), ac)
                           == constraint_fix_list.end());
                constraint_fix_list.push_back(ac);
                to_invoke_fix.push_back(-1);
            }
            break;
            case Trigger_Change:
            {
                D_ASSERT(std::find(constraint_change_list.begin(),
                                   constraint_change_list.end(), ac)
                            == constraint_change_list.end());
                constraint_change_list.push_back(ac);
                to_invoke_change.push_back(std::set<int>());
            }
            break;
            case Trigger_RBaseFinished:
            {
                D_ASSERT(std::find(constraint_rbase_finished_list.begin(),
                                   constraint_rbase_finished_list.end(), ac)
                            == constraint_rbase_finished_list.end());
                constraint_rbase_finished_list.push_back(ac);
            }
            break;
            default:
              abort();
        }
    }

    SplitState triggerSplit(int oldcell, int newcell, int oldcellsize, int newcellsize)
    {
        tracer->addEvent(BranchEvent(oldcell, newcell, oldcellsize, newcellsize));
        if(oldcellsize == 1)
            triggerFix(oldcell);
        else if(newcellsize == 1)
            triggerFix(newcell);

        triggerChange(oldcell,newcell);

        return SplitState(true);
    }

    virtual bool hasSortData()
    { return false; }

    virtual PartitionEvent& getPartitionEvent()
    { abort(); }

    virtual void addPartitionEvent(PartitionEvent se)
    {
        tracer->addPartitionEvent(std::move(se));
    }

private:
    void triggerChange(int oldcell, int newcell)
    {
        for(auto & i : to_invoke_change)
        {
            i.insert(oldcell);
            i.insert(newcell);
        }
    }

    void triggerFix(int fixedcell)
    {
        for(int & i : to_invoke_fix)
            if(i == -1)
                i = fixedcell;
    }
public:
    SplitState invokeQueue()
    {
        vec1<int> buffer;
        bool triggered = true;
        while(triggered)
        {
            triggered = false;
            for(unsigned i = 0; i < to_invoke_change.size(); ++i)
            {
                if(!to_invoke_change[i].empty())
                {
                    tracer->beginConstraint(constraint_Change(constraint_change_list[i], to_invoke_change[i]));
                    triggered = true;
                    buffer.clear();
                    buffer.assign(to_invoke_change[i].begin(), to_invoke_change[i].end());
                    to_invoke_change[i].clear();
                    info_out(2, "Executing: " << constraint_change_list[i]->full_name());
                    if(constraint_change_list[i]->signal_changed_buildingRBase(buffer).hasFailed())
                        return SplitState(false);
                    info_out(2, "After splitting: " <<
                                constraint_change_list[i]->getPartitionStack()->dumpCurrentPartition());
                    tracer->endConstraint();
                }
            }

            for(unsigned i = 0; i < to_invoke_fix.size(); ++i)
            {
                if(to_invoke_fix[i] != -1)
                {
                    tracer->beginConstraint(constraint_Fix(constraint_fix_list[i]));
                    triggered = true;
                    to_invoke_fix[i] = -1;
                    info_out(2, "Executing: " << constraint_fix_list[i]->full_name());
                    if(constraint_fix_list[i]->signal_fix_buildingRBase().hasFailed())
                        return SplitState(false);
                    info_out(2, "After splitting: " <<
                                constraint_fix_list[i]->getPartitionStack()->dumpCurrentPartition());
                    tracer->endConstraint();
                }
            }
        }
        return SplitState(true);
    }

    void RBaseFinished(RBase* rb)
    {
        for(auto & i : constraint_rbase_finished_list)
        {
            i->signal_RBaseFinished(rb);
        }
    }
};

#endif
