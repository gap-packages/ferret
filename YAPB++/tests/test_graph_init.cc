#include "problem.hpp"
#include "constraints/slowgraph.hpp"
#include <iostream>

int main(void)
{
    Problem p(12);
    vec1<vec1<int> > s(10); //{{2,3},{1,3},{1,2},{5},{6},{7},{8},{9,10}};
    int vals[][3] = {{0},{2,3,0},{1,3,0},{1,2,0},{5,0},{6,0},{7,0},{8,0},{9,10,0},{1,0}};
    for(int i = 1; i <= s.size(); ++i)
    {
        for(int j = 0; vals[i][j] != 0; ++j)
            s[i].push_back(vals[i][j]);
    }
    p.addConstraint(new SlowGraph<GraphDirected_yes>(s, &p.p_stack));
    p.p_stack.sanity_check();
    p.init();
    p.p_stack.sanity_check();
    assert(p.p_stack.cellCount() == 10);
    p.con_queue.invokeQueue();
    p.p_stack.sanity_check();
    assert(p.p_stack.cellCount() == 10);
}
