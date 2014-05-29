#include "problem.hpp"


int main(void)
{
    Permutation p;
    D_ASSERT(p[1] == 1);
    D_ASSERT(p[100] == 100);

    vec1<int> v;
    v.push_back(2);
    v.push_back(1);
    Permutation p2(v);
    D_ASSERT(p2[1] == 2);
    D_ASSERT(p2[2] == 1);
    D_ASSERT(p2[3] == 3);

    p = p2;

    Permutation p3(v);

    p2 = p3;
    p3 = p3;

    vec1<int> v2;

    Permutation p4(v2);
    D_ASSERT(p4[1] == 1);
    D_ASSERT(p4[2] == 2);
    D_ASSERT(p4.size() == 0);
}