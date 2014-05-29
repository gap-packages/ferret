#include "problem.hpp"
#include <iostream>

int main(void)
{
    Problem p(3);
    p.p_stack.sanity_check();
    p.p_stack.split(1,2);
    p.p_stack.sanity_check();
    assert(p.p_stack.fixed_cells().size() == 1);
    assert(p.p_stack.fixed_cells()[1] == 1);
    assert(*(p.p_stack.cellStartPtr(1)) == 1);
}