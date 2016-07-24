#include "problem.hpp"
#include <iostream>

int main(void)
{
    Problem p(10);
    p.p_stack.sanity_check();
    p.p_stack.split(1,5);
    p.p_stack.sanity_check();
    vec1<vec1<int> > state1 = p.p_stack.dumpCurrentPartition();
    p.full_search_memory_backtracker.pushWorld();
    assert(state1 == p.p_stack.dumpCurrentPartition());
    p.p_stack.sanity_check();
    p.p_stack.split(1,2);
    p.p_stack.sanity_check();
    p.p_stack.split(2,7);
    assert(p.p_stack.cellCount() == 4);
    vec1<vec1<int> > state2 = p.p_stack.dumpCurrentPartition();
    p.full_search_memory_backtracker.pushWorld();
    assert(state2 == p.p_stack.dumpCurrentPartition());
    p.p_stack.sanity_check();
    p.full_search_memory_backtracker.popWorld();
    assert(state2 == p.p_stack.dumpCurrentPartition());
    p.full_search_memory_backtracker.popWorld();
    p.p_stack.sanity_check();
    assert(state1 == p.p_stack.dumpCurrentPartition());
}