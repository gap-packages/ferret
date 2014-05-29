#ifndef ABSTRACT_QUEUE_HPP
#define ABSTRACT_QUEUE_HPP

#include "constraints/abstract_constraint.hpp"
#include "triggering_events.hpp"

#include "library/sort_events.hpp"


struct AbstractQueue
{
    virtual void addTrigger(AbstractConstraint* ac, TriggerType tt) = 0;
    virtual SplitState triggerSplit(int oldcell, int newcell, int oldcellsize, int newcellsize) = 0;


    virtual bool hasSortData() = 0;
    virtual PartitionEvent& getPartitionEvent() = 0;
    virtual void addPartitionEvent(PartitionEvent se) = 0;
};

#endif
