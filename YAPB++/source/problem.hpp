#ifndef _PROBLEM_PBAQLOEHZX
#define _PROBLEM_PBAQLOEHZX

#include "memory_backtrack.hpp"
#include "constraint_store.hpp"
#include "partition_stack.hpp"
#include "queue/tracer.hpp"
#include "queue/rbase_generating_queue.hpp"

struct Problem
{
    MemoryBacktracker memory_backtracker;
    TracerGenerator tracer_generator;
    ConstraintStore con_store;
    ConstraintQueue con_queue;
    PartitionStack p_stack;
    Problem(int n) :
    tracer_generator(&memory_backtracker),
    con_store(this),
    con_queue(&tracer_generator),
    p_stack(n, &con_queue, &memory_backtracker)
    {
        if(n < 2)
        {
            std::cerr << "YAPB++ requires problems on at least 2 points. Sorry" << std::endl;
            std::cerr << "Super fatal error!" << std::endl;
            abort();
        }
    }

    void addConstraint(AbstractConstraint* ac)
    { con_store.addConstraint(ac); }

    void init()
    {
        con_store.initConstraints();
        // We do not care about the initalization, we do not want it filling
        // up our trace (at least for groups, let's cover co-sets later...)
        tracer_generator.clearTrace();
    }
};

#include "constraint_store.ipp"

#endif
