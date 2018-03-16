#include "problem.hpp"
#include "constraints/setstab.hpp"
#include "search/search.hpp"
#include <iostream>

int main(void)
{
    Problem p(6);
    std::set<int> s; // c++14 {2,4};
    s.insert(2);
    s.insert(4);

    std::vector<AbstractConstraint*> v;
    v.push_back(new SetStab(s, &p.p_stack));
    SearchOptions so;
    so.only_find_generators = false;
    SolutionStore ss = doSearch(&p, v, so);

    D_ASSERT(ss.sols().size() == 4*3*2*2);
    for(int i : range1(ss.sols().size()))
    {
        D_ASSERT(ss.sols()[i][2] == 2 || ss.sols()[i][2] == 4);
        D_ASSERT(ss.sols()[i][2] == 4 || ss.sols()[i][2] == 2);
    }
}
