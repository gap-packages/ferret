#ifndef _CONSTRAINT_STORE_PLAD
#define _CONSTRAINT_STORE_PLAD

#include "constraints/abstract_constraint.hpp"
#include "queue/rbase_generating_queue.hpp"
#include "library/library.hpp"
#include "library/perm.hpp"

class ConstraintStore
{
    vec1<AbstractConstraint*> constraints;
    bool constraints_initalized;
public:

    typedef const vec1<AbstractConstraint*>* get_type;
    const vec1<AbstractConstraint*>* get() const
    { return &constraints; }

    ConstraintStore()
    : constraints_initalized(false)
    { }

    void addConstraint(AbstractConstraint* con)
    {
        D_ASSERT(!constraints_initalized);
        constraints.push_back(con);
    }

    void initConstraints(ConstraintQueue* cq)
    {
        D_ASSERT(!constraints_initalized);
        constraints_initalized = true;
        for(int i = 1; i <= constraints.size(); ++i)
        {
            constraints[i]->init();
            SplitState ss = cq->invokeQueue();
            (void)ss; // Avoid warning
            D_ASSERT(!ss.hasFailed());
        }
    }

    ~ConstraintStore()
    {
        for(int i = 1; i <= constraints.size(); ++i)
            delete constraints[i];
    }

    bool initCalled() const
    { return constraints_initalized; }

    bool verifySolution(const Permutation& p) const
    {
        // TODO: Investigate cases where this fails
        static bool printed_warning = false;
        for(int i = 1; i <= constraints.size(); ++i)
        {
            if(!constraints[i]->verifySolution(p))
            {
                if(!printed_warning)
                {
                    printed_warning = true;
                    std::cerr << "A solution has failed checking!\n";
                    std::cerr << "This was caused by a " << constraints[i]->name() << " constraint\n";
                    std::cerr << "Your answer will hopefully still be correct,\n";
                    std::cerr << "But please report your problem\n";
                }
             //   assert(0);
                return false;
            }
        }
        return true;
    }
};

#endif
