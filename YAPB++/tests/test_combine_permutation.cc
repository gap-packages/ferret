#include "library/perm.hpp"
#include "library/algorithms.hpp"

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
    D_ASSERT(!(p == p2));

    vec1<Permutation> vp;
    vp.push_back(p);
    vp.push_back(p2);
    D_ASSERT(p2 == mergePermutations(vp));

    vec1<Permutation> vp2;
    vp2.push_back(p2);
    vp2.push_back(p2);
    D_ASSERT(p == mergePermutations(vp2));

    v.clear();
    v.push_back(1);
    v.push_back(3);
    v.push_back(2);
    Permutation p3(v);

    PermutationStack ps;
    ps.addPerm(p2);
    ps.addPerm(p3);
    D_ASSERT(ps[1] == 3);
    D_ASSERT(ps[2] == 1);
    D_ASSERT(ps[3] == 2);
    D_ASSERT(ps[4] == 4);

    vec1<Permutation> vp3;
    vp3.push_back(p2);
    vp3.push_back(p3);
    D_ASSERT(ps.getPerm() == mergePermutations(vp3));

    p2 = p3;
    p3 = p3;

    vec1<int> v2;

    Permutation p4(v2);
    D_ASSERT(p4[1] == 1);
    D_ASSERT(p4[2] == 2);
    D_ASSERT(p4.size() == 0);
}