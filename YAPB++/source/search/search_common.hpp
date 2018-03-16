#ifndef SEARCH_COMMON_HPP
#define SEARCH_COMMON_HPP

#include "problem.hpp"
#include "rbase/build_rbase.hpp"
#include "queue/trace_following_queue.hpp"
#include "solution_store.hpp"
#include "search/search_options.hpp"
#include "library/stats.hpp"
#include "library/perm.hpp"

// Checks a solution satisfies all the constraints, and
// adds to the solutionStore if it is. Returns true if
// the solution is real
bool handlePossibleSolution(Problem* p, SolutionStore* ss, RBase* rbase)
{
    D_ASSERT(p->p_stack.cellCount() == p->p_stack.domainSize());
    Permutation perm = getRawPermutation(p->p_stack.domainSize());

    for(int i : range1(perm.size()) )
    {
        perm.raw(rbase->initial_permstack->val(i)) = p->p_stack.val(i);
    }
    D_ASSERT(perm.validate());
    if(!p->con_store.verifySolution(perm))
        return false;
    info_out(1, "Solution: " << perm);
    ss->addSolution(perm);
    return true;
}

// Orders a cell of a partition stack according to a given heuristic

template<typename It>
void orderCell(It begin, It end, SearchHeuristic sh, RBase* rbase)
{
    switch(sh)
    {
        case SearchBranch_RBase:
             std::sort(begin, end,
                IndirectSorter([&](auto i) -> auto& { return (rbase->inv_value_ordering)[i]; }));
            return;
        case SearchBranch_InvRBase:
             std::sort(begin, end,
                ReverseSorter(IndirectSorter([&](auto i) -> auto& { return (rbase->inv_value_ordering)[i]; })));
            return;
        case SearchBranch_Random:
            std::random_shuffle(begin, end);
            return;
        case SearchBranch_Sorted:
            std::sort(begin, end);
            return;
        case SearchBranch_Nosort:
            return;
        default:
            abort();
    }
}

#endif
