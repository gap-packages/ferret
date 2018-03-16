#include "problem.hpp"
#include "constraints/edgecolouredgraph.hpp"
#include "search/search.hpp"
#include <iostream>

int main(void)
{
    Problem p(6);
    vec1<vec1<UncolouredEdge> > s(6); // c++14 : {{2},{3},{1},{5},{6},{4}};
    int vals[] = {0,2,3,1,5,6,4};
    for(int i : range1(6))
    	s[i].push_back(vals[i]);
    std::vector<AbstractConstraint*> v;
    v.push_back(new EdgeColouredGraph<UncolouredEdge, GraphDirected_yes>(s, GraphConfig(), &p.p_stack));
    SearchOptions so;
    so.only_find_generators = false;
    SolutionStore ss = doSearch(&p, v, so);

    D_ASSERT(ss.sols().size() ==3*3*2);
}
