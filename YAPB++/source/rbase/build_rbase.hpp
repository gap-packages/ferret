#ifndef BUILD_RBASE_HPPAC
#define BUILD_RBASE_HPPAC

#include "rbase.hpp"
#include "problem.hpp"
#include "search/search_options.hpp"

// The purpose of an RBase Generator is that it will backtrack,
// so we can explore different options. Once we have finalised
// our RBase, we fix it.
class BacktrackingRBase
{
    RevertingStack<int> branchcell;
    RevertingStack<int> branchvalue;


    BacktrackingRBase(const BacktrackingRBase&);
public:
    BacktrackingRBase(MemoryBacktracker* mb)
    : branchcell(mb->makeRevertingStack<int>()),
      branchvalue(mb->makeRevertingStack<int>())
    {}


    void addBranch(int cell, int value)
    {
        branchcell.push_back(cell);
        branchvalue.push_back(value);
    }

    int size() const
    { return branchcell.size(); }



    RBase* fixRBase(PartitionStack* ps, const vec1<TraceList>& trace)
    {
        RBase* rb = new RBase();
        rb->branchcell = branchcell.get();
        rb->branchvalue = branchvalue.get();
        rb->initial_permstack = ps->clone();
        rb->trace = trace;
        //vec1<int> ordering(ps->domainSize(), -1);
        //for(int i : range1(ps->domainSize()))
        //    ordering[*(ps->cellStartPtr(i))] = i;
        rb->value_ordering = ps->fixed_values(); //ordering;
        rb->inv_value_ordering = invertVecAsPermutation(rb->value_ordering);
        D_ASSERT(ps->fixed_values().size() == ps->domainSize());
        return rb;
    }
};

int choose_branch_cell(PartitionStack* ps, ConstraintStore* cstore,
                       RBaseSearchHeuristic sh)
{
    int branch_cell = 1;
    switch(sh)
    {
        case RBaseBranch_First:
        {
            while(branch_cell <= ps->cellCount() &&
                  ps->cellSize(branch_cell) == 1)
                    branch_cell++;
            if(branch_cell <= ps->cellCount())
                return branch_cell;
            else
                return -1;
        }
        case RBaseBranch_Random:
        {
            // This does not choose evenly between all non-unit cells, as we jump to a random point
            // and search from there.
            int branch_start = 1 + (random() % ps->cellCount());
            branch_cell = branch_start;
            while(branch_cell <= ps->cellCount() &&
                  ps->cellSize(branch_cell) == 1)
                    branch_cell++;

            if(branch_cell <= ps->cellCount())
                return branch_cell;

            branch_cell = 1;
            while(branch_cell < branch_start &&
                  ps->cellSize(branch_cell) == 1)
                    branch_cell++;

            if(branch_cell < branch_start)
                return branch_cell;
            else
                return -1;
        }
        // Super note: We do 'fall through' on this case into RBaseBranch_Smallest!
        case RBaseBranch_ConstraintAdvise:
        {
            ConstraintStore::get_type container = cstore->get();
            for(int i : range1(container->size()))
            {
                int val = ((*container)[i])->advise_branch();
                if(val != -1)
                    return val;
            }
        } // NOTE: There is no 'break' or 'return' here on purpose
        case RBaseBranch_Smallest:
        {
            int best_cell = -1;
            int best_size = -1;
            while(branch_cell <= ps->cellCount())
            {
                if(ps->cellSize(branch_cell) != 1)
                {
                    if(best_size == -1 || ps->cellSize(branch_cell) < best_size)
                    {
                        best_size = ps->cellSize(branch_cell);
                        best_cell = branch_cell;
                    }
                }
                branch_cell++;
            }

            return best_cell;
        }
        case RBaseBranch_RandomSmallest:
        {
            // First find the smallest value.
            vec1<int> best_cells;
            int best_size = -1;
            while(branch_cell <= ps->cellCount())
            {
                if(ps->cellSize(branch_cell) != 1)
                {
                    if(best_size == -1 || ps->cellSize(branch_cell) < best_size)
                    {
                        best_size = ps->cellSize(branch_cell);
                        best_cells.clear();
                        best_cells.push_back(branch_cell);
                    }
                    else if(ps->cellSize(branch_cell) == best_size)
                    {
                        best_cells.push_back(branch_cell);
                    }
                }
                branch_cell++;
            }
            if(best_cells.empty())
                return -1;
            else
                return best_cells[(random() % best_cells.size())+1];
        }
        case RBaseBranch_Largest:
        {
            int best_cell = -1;
            int best_size = -1;
            while(branch_cell <= ps->cellCount())
            {
                if(ps->cellSize(branch_cell) != 1)
                {
                    if(ps->cellSize(branch_cell) > best_size)
                    {
                        best_size = ps->cellSize(branch_cell);
                        best_cell = branch_cell;
                    }
                }
                branch_cell++;
            }

            return best_cell;
        }
        case RBaseBranch_Smallest2:
        {
            int best_cell = -1;
            int best_size = -1;
            int second_best_cell = -1;
            int second_best_size = -1;
            while(branch_cell <= ps->cellCount())
            {
                int size = ps->cellSize(branch_cell);
                if(size != 1)
                {
                    if(best_size == -1 || size < best_size)
                    {
                        best_size = size;
                        best_cell = branch_cell;
                    }
                    if(size > best_size && (second_best_size == -1 || second_best_size > size))
                    {
                        second_best_size = size;
                        second_best_cell = branch_cell;
                    }
                }
                branch_cell++;
            }

            if(second_best_cell != -1)
                return second_best_cell;
            else
                return best_cell;
        }

        default:
        abort();
    }
}

template<typename It>
It choose_value(It begin, It end, RBaseSearchHeuristic sh)
{
    switch(sh)
    {
        case RBaseBranch_First:
            return begin;
        case RBaseBranch_Largest:
            return std::max_element(begin, end);
        case RBaseBranch_Smallest:
            return std::min_element(begin, end);
        case RBaseBranch_Smallest2:
            throw std::runtime_error("Smallest2 not implemented as rBase value heuristic");
        case RBaseBranch_Random:
            return random() % (end - begin) + begin;
        case RBaseBranch_RandomSmallest:
            throw std::runtime_error("RandomSmallest is not valid for rBase value heuristic");
        default:
            abort();
    }
}

RBase* buildRBase(Problem* p, RBaseSearchHeuristic cellHeuristic, RBaseSearchHeuristic valueHeuristic)
{
    int depth = p->full_search_memory_backtracker.getDepth();

    BacktrackingRBase revrbase(&p->full_search_memory_backtracker);
    D_ASSERT(p->con_store.initCalled());
    int branch_cell = 1;

    while(branch_cell != -1)
    {
        p->con_queue.invokeQueue();
        p->full_search_memory_backtracker.pushWorld();
        p->rbase_generation_memory_backtracker.pushWorld();
        branch_cell = choose_branch_cell(&p->p_stack,
                                         &p->con_store, cellHeuristic);
        if(branch_cell != -1)
        {
            // put a well-defined value at the start of the partition, for ease of duplication
            PartitionStack::cellit min_pos = choose_value(p->p_stack.cellStartPtr(branch_cell), p->p_stack.cellEndPtr(branch_cell), valueHeuristic);
            // (the +1 at the end is because we 1-index arrays)
            p->p_stack.swapPositions(p->p_stack.cellStartPos(branch_cell), min_pos - p->p_stack.valStart()+1);

            int cell_start = p->p_stack.cellStartPos(branch_cell);
            info_out(1, "RBase Level " <<  revrbase.size()+1 << " : " <<  p->p_stack.val(cell_start) << ", location " << cell_start << ", in cell " << branch_cell << ", size " << p->p_stack.cellSize(branch_cell));
            Stats::container().rBase_fixed_points.push_back(std::make_pair(branch_cell, p->p_stack.cellSize(branch_cell)));
            info_out(1, "RBase Cell starts: " << p->p_stack.cellStarts() << ", lengths: " << p->p_stack.cellSizes());
            revrbase.addBranch(branch_cell, p->p_stack.val(cell_start));
            p->p_stack.split(branch_cell, cell_start + 1);
        }
    }
    info_out(1, "Finished RBase building");

    D_ASSERT(p->p_stack.cellCount() == p->p_stack.domainSize());

    RBase* rb = revrbase.fixRBase(&p->p_stack, p->tracer_generator.getTrace());
    p->con_queue.RBaseFinished(rb);
    p->full_search_memory_backtracker.popWorldToDepth(depth);
    return rb;
}



#endif
