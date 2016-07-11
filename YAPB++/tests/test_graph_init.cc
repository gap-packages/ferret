#include "problem.hpp"
#include "constraints/edgecolouredgraph.hpp"
#include <iostream>

int main(void)
{
    Problem p(10);
    vec1<vec1<UncolouredEdge> > s(10);
    int vals[][4] = {{0},{2,3,0},{1,3,0},{1,2,8,0},{5,0},{6,0},{7,0},{8,0},{9,0},{2,10,0}, {2,5,0}};
    for(int i : range1(s.size()))
    {
        for(int j = 0; vals[i][j] != 0; ++j)
            s[i].push_back(vals[i][j]);
    }
    p.addConstraint(new EdgeColouredGraph<UncolouredEdge, GraphDirected_yes>(s, GraphConfig(), &p.p_stack));
    p.p_stack.sanity_check();
    p.con_store.initConstraints(true);
    p.p_stack.sanity_check();
    assert(p.p_stack.cellCount() == 10);
    p.con_queue.invokeQueue();
    p.p_stack.sanity_check();
    assert(p.p_stack.cellCount() == 10);
}
