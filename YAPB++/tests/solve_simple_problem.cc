#include "problem.hpp"
#include "constraints/setstab.hpp"
#include "search.hpp"
#include <iostream>

int main(void)
{
    Problem p(6);
    std::set<int> s; // c++11 {2,4};
    s.insert(2);
    s.insert(4);

    p.addConstraint(new SetStab(s, &p.p_stack));
    SolutionStore ss = doSearch(&p, SearchOptions());

    D_ASSERT(ss.sols().size() == 4*3*2*2);
    for(int i = 1; i <= ss.sols().size(); ++i)
    {
        D_ASSERT(ss.sols()[i][2] == 2 || ss.sols()[i][2] == 4);
        D_ASSERT(ss.sols()[i][2] == 4 || ss.sols()[i][2] == 2);
    }
}
