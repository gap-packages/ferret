#ifndef _SEARCH_HPP_CAW
#define _SEARCH_HPP_CAW


#include "search/search_common.hpp"

class EndOfSearch : public std::exception
{ };

template<bool firstbranch>
bool doSearchBranch(const SearchOptions& so, Problem* p, SolutionStore* ss,
                    RBase* rbase, TraceFollowingQueue* tfq, int depth)
{
    info_out(1, "search depth: " << depth);
    info_out(2, "Current partition: " <<  p->p_stack.dumpCurrentPartition());
    if(depth > rbase->depth())
    {
        info_out(1, "Reached bottom of search");
        return handlePossibleSolution(p,ss,rbase);
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

        info_out(1, "branching on cell: " << cell);
        // if we are on the first branch, doing a group search, the first value we branch on must be the rbase
        // value, so we don't pass it to the sorter

        orderCell(firstbranch ? cell.begin() + 1 : cell.begin(),
                  cell.end(),
                  firstbranch ? so.heuristic.search_first_branch_value : so.heuristic.search_value,
                  rbase);

        for(int i : range1(cell.size()))
        {
            D_ASSERT(!(firstbranch && i == 1 && !ss->isMinimum(cell[i])));
            D_ASSERT(!(firstbranch && i == 1 && cell[i] != rbase->branchvalue[depth]));
            info_out(1, "consider branching on: " << cell[i]);
            if(!firstbranch || !so.only_find_generators || ss->isMinimum(cell[i]))
            {
                // Find value we are branching on
                int i_pos = p->p_stack.invval(cell[i]);

                // Put it at the start of the partition
                p->p_stack.swapPositions(cell_start, i_pos);

                // Push state of the world
                p->full_search_memory_backtracker.pushWorld();
                info_out(1, "branch on: " << cell[i]);
                Stats::container().node_count++;
                if(so.node_limit >= 0 && Stats::container().node_count >= so.node_limit)
                { throw EndOfSearch(); }
                tfq->beginBranch();
                SplitState branch_split = p->p_stack.split(branchcell, cell_start + 1);
                (void)branch_split;
                D_ASSERT(!branch_split.hasFailed());
                tfq->endBranch();
                debug_out(3, "search", "Perm State " << p->p_stack.printCurrentPartition());


                SplitState split = tfq->execute_trace();
                if(!split.hasFailed())
                {
                    Stats::container().bad_leaves++;
                    bool ret;
                    if(firstbranch && i == 1)
                        ret = doSearchBranch<true>(so, p, ss, rbase, tfq, depth + 1);
                    else
                        ret = doSearchBranch<false>(so, p, ss, rbase, tfq, depth + 1);
                    if(so.only_find_generators && ret)
                    {
                        if(!firstbranch)
                        {
                            p->full_search_memory_backtracker.popWorld();
                            return true;
                        }
                        else
                        {
                            ss->markLastSolutionFrom(cell[1], cell[i]);
                        }
                    }
                }
                p->full_search_memory_backtracker.popWorld();
            }
            else
            { debug_out(1, "search", "skipping " << i); }
        }
        info_out(1, "backtracking");
        if(!firstbranch)
            Stats::container().bad_internal_nodes++;
        return false;
    }

}

SolutionStore doSearch(Problem* p, const std::vector<AbstractConstraint*>& cons, const SearchOptions& so)
{
    Stats::reset();
    timing_reset();
    for(auto con : cons) p->addConstraint(con);
    p->con_store.initConstraints(true);
    p->tracer_generator.clearTrace();
    RBase* rb = buildRBase(p, so.heuristic.rbase_cell, so.heuristic.rbase_value);
    Stats::container().rBase_value_ordering = rb->value_ordering;
    timing_event("Finish RBase");
    SolutionStore solutions(rb);
    if(!so.just_rbase)
    {

        TraceFollowingQueue tfq(rb->trace, &p->full_search_memory_backtracker);

        p->p_stack.setAbstractQueue(&tfq);

        Stats::container().node_count = 0;
        try {
            doSearchBranch<true>(so, p, &solutions, rb, &tfq, 1);
        }
        catch(const EndOfSearch&) { 
            debug_out(1, "search", "Node limit reached!");
        }
    }
    debug_out(1, "search", "Node count:" << Stats::container().node_count);
    delete rb;

    RECORD_STATS(dumpStats());

    return solutions;
}

SolutionStore doCosetSearch(Problem* p, const std::vector<AbstractConstraint*>& consCommon,
                                        const std::vector<AbstractConstraint*>& consL,
                                        const std::vector<AbstractConstraint*>& consR,
                                        const SearchOptions& so)
{
    Stats::reset();
(void)consL;
(void)consR;
    for(auto i : consCommon)
    {
      p->addConstraint(i);
    }
    p->con_store.initConstraints(true);
    p->tracer_generator.clearTrace();
    RBase* rb = buildRBase(p, so.heuristic.rbase_cell, so.heuristic.rbase_value);
    Stats::container().rBase_value_ordering = rb->value_ordering;
    timing_event("Finish RBase");
    SolutionStore solutions(rb);
    if(!so.just_rbase)
    {

        TraceFollowingQueue tfq(rb->trace, &p->full_search_memory_backtracker);

        p->p_stack.setAbstractQueue(&tfq);

        Stats::container().node_count = 0;
        doSearchBranch<true>(so, p, &solutions, rb, &tfq, 1);
    }
    debug_out(1, "search", "Node count:" << Stats::container().node_count);
    delete rb;

    RECORD_STATS(dumpStats());

    return solutions;
}

#endif
