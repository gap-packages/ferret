#ifndef _SEARCH_HPP_CAW
#define _SEARCH_HPP_CAW

#include "problem.hpp"
#include "rbase/build_rbase.hpp"
#include "queue/trace_following_queue.hpp"
#include "solution_store.hpp"
#include "search_options.hpp"
#include "library/stats.hpp"
#include "library/perm.hpp"

bool handleSolution(Problem* p, SolutionStore* ss, RBase* rbase)
{
    D_ASSERT(p->p_stack.cellCount() == p->p_stack.domainSize());
    Permutation perm = getRawPermutation(p->p_stack.domainSize());

    for(int i = 1 ; i <= perm.size(); ++i)
    {
        perm.raw(rbase->initial_permstack->val(i)) = p->p_stack.val(i);
    }
    D_ASSERT(perm.validate());
    if(!p->con_store.verifySolution(perm))
        return false;
    ss->addSolution(perm);
    return true;
}

template<typename It>
void orderCell(It begin, It end, SearchHeuristic sh, RBase* rbase)
{
    switch(sh)
    {
        case SearchBranch_RBase:
             std::sort(begin, end,
                IndirectSorter(ContainerToFunction(&rbase->inv_value_ordering)));
            return;
        case SearchBranch_InvRBase:
             std::sort(begin, end,
                ReverseSorter(IndirectSorter(ContainerToFunction(&rbase->inv_value_ordering))));
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

template<bool firstbranch>
bool doSearchBranch(const SearchOptions& so, Problem* p, SolutionStore* ss,
                    RBase* rbase, TraceFollowingQueue* tfq, int depth)
{
    debug_out(1, "Search", "starting depth: " << depth);
    if(depth > rbase->depth())
    {
        debug_out(1, "Search", "found solution");
        return handleSolution(p,ss,rbase);
    }
    else
    {
        int branchcell = rbase->branchcell[depth];
        int cell_start = p->p_stack.cellStartPos(branchcell);

        if(firstbranch)
        {
            int first_val = rbase->branchvalue[depth];
            int first_val_pos = p->p_stack.invval(first_val);
            p->p_stack.swapPositions(first_val_pos, cell_start);
        }

        // We need to really make a copy of this, else we have problems
        // with iterating through the cell in a sensible method.
        vec1<int> cell = p->p_stack.cellAsVec(branchcell);

        // if we are on the first branch, doing a group search, the first value we branch on must be the rbase
        // value, so we don't pass it to the sorter

        orderCell(firstbranch ? cell.begin() + 1 : cell.begin(),
                  cell.end(),
                  firstbranch ? so.search_first_branch_value_heuristic : so.search_value_heuristic,
                  rbase);

        for(int i = 1; i <= cell.size(); ++i)
        {
            D_ASSERT(!(firstbranch && i == 1 && !ss->isMinimum(cell[i])));
            D_ASSERT(!(firstbranch && i == 1 && cell[i] != rbase->branchvalue[depth]));
            debug_out(1, "search", "consider branching on: " << cell[i]);
            if(!firstbranch || !so.only_find_generators || ss->isMinimum(cell[i]))
            {
                // Find value we are branching on
                int i_pos = p->p_stack.invval(cell[i]);

                // Put it at the start of the partition
                p->p_stack.swapPositions(cell_start, i_pos);

                // Push state of the world
                p->memory_backtracker.pushWorld();
                debug_out(1, "search", "branch on: " << cell[i]);
                Stats::container().node_count++;
                tfq->beginBranch();
                SplitState branch_split = p->p_stack.split(branchcell, cell_start + 1);
                (void)branch_split;
                D_ASSERT(!branch_split.hasFailed());
                tfq->endBranch();
                debug_out(0, "Search", "Perm State " << p->p_stack.printCurrentPartition());


                SplitState split = tfq->execute_trace();
                if(!split.hasFailed())
                {
                    Stats::container().bad_leaves++;
                    bool ret;
                    if(firstbranch && i == 1)
                        ret = doSearchBranch<true>(so, p, ss, rbase, tfq, depth + 1);
                    else
                        ret = doSearchBranch<false>(so, p, ss, rbase, tfq, depth + 1);
                    if(!firstbranch && so.only_find_generators && ret)
                    {
                        p->memory_backtracker.popWorld();
                        return true;
                    }
                }
                p->memory_backtracker.popWorld();
            }
            else
            { debug_out(1, "search", "skipping " << i); }
        }
        debug_out(1, "Search", "backtrack");
        if(!firstbranch)
            Stats::container().bad_internal_nodes++;
        return false;
    }

}

SolutionStore doSearch(Problem* p, const SearchOptions& so)
{
    Stats::reset();

    timing_start();
    p->init();
    RBase* rb = buildRBase(p, so.rbase_cell_heuristic, so.rbase_value_heuristic);
    Stats::container().fixed_points = rb->value_ordering;
    timing_event("Finish RBase");
    SolutionStore solutions(rb);
    if(!so.just_rbase)
    {

        TraceFollowingQueue tfq(rb->trace, &p->memory_backtracker);

        p->p_stack.setAbstractQueue(&tfq);

        Stats::container().node_count = 0;
        doSearchBranch<true>(so, p, &solutions, rb, &tfq, 1);
    }
    timing_finish();
    debug_out(1, "search", "Node count:" << Stats::container().node_count);
    delete rb;

    RECORD_STATS(dumpStats());

    return solutions;
}

#endif
