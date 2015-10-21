#ifndef _CONSTRAINT_STORE_PLAD
#define _CONSTRAINT_STORE_PLAD

#include "constraints/abstract_constraint.hpp"
#include "queue/rbase_generating_queue.hpp"
#include "library/library.hpp"
#include "library/perm.hpp"

struct Problem;

class ConstraintStore
{
    Problem* p;
    vec1<AbstractConstraint*> constraints;
    bool constraints_initalized;
public:

    typedef const vec1<AbstractConstraint*>* get_type;
    const vec1<AbstractConstraint*>* get() const
    { return &constraints; }

    ConstraintStore(Problem* _p);

    void addConstraint(AbstractConstraint* con);

    void initConstraints(bool rbase_building);

    ~ConstraintStore();

    bool initCalled() const;

    bool verifySolution(const Permutation& p) const;
};

#endif
