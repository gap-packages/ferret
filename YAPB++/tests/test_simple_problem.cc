#include "problem.hpp"
#include "constraints/setstab.hpp"
#include <iostream>

int main(void)
{
    Problem p(11);
    std::set<int> s; // c++14: {2,4,6,8};
    s.insert(2); s.insert(4); s.insert(6); s.insert(8);
    p.addConstraint(new SetStab(s, &p.p_stack));
    p.p_stack.sanity_check();
    p.con_store.initConstraints(true);
    p.p_stack.sanity_check();
    assert(p.p_stack.cellCount() == 2);
    assert(p.p_stack.cellSize(2) == 4);
}
