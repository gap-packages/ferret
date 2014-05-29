#include "problem.hpp"
#include <iostream>

int main(void)
{
    Problem p(20);
    p.p_stack.sanity_check();
    p.p_stack.split(1,10);
    p.p_stack.sanity_check();
    p.p_stack.split(1,5);
    p.p_stack.sanity_check();
    p.p_stack.split(2,15);
    p.p_stack.sanity_check();
}