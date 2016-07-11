#include "constraint_store.hpp"

ConstraintStore::ConstraintStore(Problem* _p)
    : p(_p), constraints_initalized(false)
    { }

void ConstraintStore::addConstraint(AbstractConstraint* con)
    {
        D_ASSERT(!constraints_initalized);
        con->setId(toString(constraints.size()));
        constraints.push_back(con);
    }

void ConstraintStore::initConstraints(bool rbase_building)
    {
        D_ASSERT(!constraints_initalized);
        constraints_initalized = true;
        for(auto con : constraints)
        {
            std::vector<TriggerType> v = con->triggers();
            for(auto & j : v)
              p->p_stack.addTrigger(con, j);

            if(rbase_building)
              con->signal_start_buildingRBase();
            else
              con->signal_start();

            SplitState ss = p->con_queue.invokeQueue();
            (void)ss; // Avoid warning
            D_ASSERT(!ss.hasFailed());
        }
    }

ConstraintStore::~ConstraintStore()
    {
        for(auto con : constraints)
            delete con;
    }

bool ConstraintStore::initCalled() const
    { return constraints_initalized; }

bool ConstraintStore::verifySolution(const Permutation& p) const
    {
        // TODO: Investigate cases where this fails
        static bool printed_warning = false;
        for(int i : range1(constraints.size()))
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
