#include "rbase/build_rbase.hpp"
#include "constraints/setstab.hpp"

int main(void)
{
    Problem p(10);
    int vals[] = {2,4,6,8};
    std::set<int> s(vals, vals+4);
    p.addConstraint(new SetStab(s, &p.p_stack));
    p.p_stack.sanity_check();
    p.con_store.initConstraints(true);
    p.p_stack.sanity_check();
    vec1<vec1<int> > part = p.p_stack.dumpCurrentPartition();
    RBase* r = buildRBase(&p, RBaseBranch_Smallest, RBaseBranch_Smallest);
    // make sure partition is not changed
    assert(part == p.p_stack.dumpCurrentPartition());
    delete r;
}
