#include "problem.hpp"
#include "constraints/slowgraph.hpp"
#include "search.hpp"
#include <iostream>

int main(void)
{
    Problem p(6);
    vec1<vec1<int> > s(6); // c++11 : {{2},{3},{1},{5},{6},{4}};
    int vals[] = {0,2,3,1,5,6,4};
    for(int i = 1; i <= 6; ++i)
    	s[i].push_back(vals[i]);
    std::vector<AbstractConstraint*> v;
    v.push_back(new SlowGraph<GraphDirected_yes>(s, &p.p_stack));
    SearchOptions so;
    so.only_find_generators = false;
    SolutionStore ss = doSearch(&p, v, so);

    D_ASSERT(ss.sols().size() ==3*3*2);
}
