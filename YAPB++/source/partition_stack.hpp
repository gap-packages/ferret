#ifndef PARTITION_STACK_CDJUCD
#define PARTITION_STACK_CDJUCD
#include <vector>
#include <assert.h>
#include <sstream>
#include <set>
#include <map>

#include "library/optional.hpp"
#include "library/algorithms.hpp"
#include "library/vec1.hpp"
#include "library/sorter.hpp"
#include "queue/abstract_queue.hpp"
#include "memory_backtrack.hpp"

/// A store of all the information we need
/// to efficently backtrack the state.
struct PartitionSplit
{
    int cell;
    int splitpos;

    PartitionSplit(int _c, int _s) :
    cell(_c), splitpos(_s)
    { }

    PartitionSplit() {}
};

struct MarkStore
{

    /// maintain data structures for cellOfVal and cellOfPos
    bool enable_cellOfFunctions;

    vec1<int> marks;


    // store mark locations (map pos -> mark)
    std::map<int, int> marks_start;

    MarkStore(int n)
    : enable_cellOfFunctions(true), marks(n+1,0)
    {
        marks[1] = 1;
        marks_start[1] = 1;
        marks[n+1] = -1;
    }

    // TODO : Make this O(cell) instead of O(n)
    int cellOfPos(int pos)
    {
        D_ASSERT(enable_cellOfFunctions);
        auto it = marks_start.upper_bound(pos);
        D_ASSERT(it != marks_start.begin());
        --it;
        return it->second;
/*
        while(marks[pos] == 0)
            pos--;
        return marks[pos];
        */
    }

};

class PartitionStack : public BacktrackableType
{
    AbstractQueue* aq;
    vec1<PartitionSplit> backtrack_stack;

    vec1<int> backtrack_depths;

    MarkStore markstore;

    /// Maximum domain value
    int n;

    // The partition, and it's inverse
    // NOTE: To help the gap<->C++ integration,
    // these are 1-based arrays (ignore value 0)
    vec1<int> vals;
    vec1<int> invvals;

    // Store explicit list of fixed cells
    vec1<int> fixed;

    // Store explicit list of the values in fixed cells
    vec1<int> fixed_vals;


    /// Starts of cells
    vec1<int> cellstart;
    /// Sizes of cells
    vec1<int> cellsize;

    /// Number of cells at push events
    vec1<int> pushes;

public:

    const vec1<int>& cellStarts()
    { return cellstart; }

    const vec1<int>& cellSizes()
    { return cellsize; }

    void setAbstractQueue(AbstractQueue* new_aq)
    { aq = new_aq; }

    AbstractQueue* getAbstractQueue()
    { return aq; }

    // This is a safety mechanism -- in general
    // copying a PartitionStack is a bad idea, but in some
    // situations we want to.
    PartitionStack* clone()
    {
        PartitionStack* ps = new PartitionStack(n, NULL, NULL);
        ps->vals = vals;
        ps->invvals = invvals;
        ps->fixed = fixed;
        ps->fixed_vals = fixed_vals;
        ps->markstore = markstore;
        ps->cellstart = cellstart;
        ps->cellsize = cellsize;
        ps->pushes = pushes;

        return ps;
    }

    void addTrigger(AbstractConstraint* ac, TriggerType tt)
    { aq->addTrigger(ac, tt); }

    typedef vec1<int>::iterator cellit;

    PartitionStack(int _n, AbstractQueue* _aq, MemoryBacktracker* mb) :
    BacktrackableType(mb), aq(_aq), markstore(_n),
      n(_n), vals(n), invvals(n)
    {
        backtrack_depths.push_back(1);

        for(int i = 1; i <= n; ++i)
        {
            vals[i] = i;
            invvals[i] = i;
        }
        cellstart.push_back(1);
        cellsize.push_back(n);
    }

    /// Get number of points in partition
    int domainSize() const
    { return n; }

    int cellCount() const
    { return cellstart.size(); }

    int cellSize(int i) const
    {
        return cellsize[i];
    }

    int cellStartPos(int i) const
    {
        return cellstart[i];
    }

    cellit cellStartPtr(int i)
    { return valPtr(cellStartPos(i)); }

    int cellEndPos(int i) const
    {
        D_ASSERT(i > 0 && i <= cellCount());
        return cellstart[i] + cellsize[i];
    }

    cellit cellEndPtr(int i)
    { return valPtr(cellEndPos(i)); }

    int val(int pos)
    { return *valPtr(pos); }

    int invval(int pos)
    { return *invvalPtr(pos); }

    const vec1<int>& val_ref() const
    { return vals; }

    const vec1<int>& invval_ref() const
    { return invvals; }

    vec1<int> cellAsVec(int cell)
    {
        return vec1<int>(cellStartPtr(cell), cellEndPtr(cell));
    }

    int cellOfVal(int val)
    {
        return cellOfPos(invvals[val]);
    }

    // TODO : Make this O(cell) instead of O(n)
    int cellOfPos(int pos)
    {
        return markstore.cellOfPos(pos);
    }

    cellit valPtr(int pos)
    { return vals.begin() + (pos - 1); }

    cellit invvalPtr(int pos)
    { return invvals.begin() + (pos - 1); }

    cellit valStart()
    { return vals.begin(); }

    cellit invvalStart()
    { return invvals.begin(); }

    const vec1<int>& fixed_cells() const
    { return fixed; }

    const vec1<int>& fixed_values() const
    { return fixed_vals; }

    /// Sanity checks all internal state of partition stack
    /// Warning: May be expensive!
    bool sanity_check()
    {
        assert(vals.size() == n);
        assert(invvals.size() == n);

        for(int i = 1; i <= n; ++i)
        {
           assert(invvals[vals[i]] == i);
           assert(vals[invvals[i]] == i);
        }

        assert(contains_no_repeats(fixed));
        assert(fixed.size() == fixed_vals.size());
        for(int i = 1; i <= fixed.size(); ++i)
        {
            assert(cellsize[fixed[i]] == 1);
            assert(*(cellStartPtr(fixed[i])) == fixed_vals[i]);
        }


        assert(contains_no_repeats(cellstart));
        assert(cellstart.size() == cellsize.size());
        assert(cellstart[1] == 1);
        assert(cellstart[1]+cellsize[1] != 0);

        int fixed_count = 0;

        for(int i = 1; i <= cellCount(); ++i)
        {
            assert(cellsize[i] > 0);
            if(cellsize[i] == 1)
                fixed_count++;
            assert(markstore.marks[cellstart[i]] == i);
            assert(cellOfPos(cellstart[i]) == i);
            assert(cellOfVal(val(cellstart[i])) == i);
            for(int j = 1; j < cellsize[i]; ++j)
            {
                assert(markstore.marks[cellstart[i] + j] == 0);
                // Re-enable these when cellOfPos gets a non-trivial
                // implementation
                assert(cellOfPos(cellstart[i] + j) == i);
                assert(cellOfVal(val(cellstart[i]+j)) == i);
            }
        }
        assert(fixed.size() == fixed_count);

        for(int i = 1; i <= n; ++i)
        {
            if(markstore.marks[i] != 0)
                assert(cellstart[markstore.marks[i]] == i);
        }
        return true;
    }


    SplitState split(int cell, int pos)
    {
        D_ASSERT(pos > cellStartPos(cell));
        D_ASSERT(pos < cellEndPos(cell));


        // We allow attempts to split an empty
        // piece off a cell, and just ignore it
//        if(pos == cellStartPos(cell) ||
  //         pos == cellEndPos(cell))
    //        return SplitState(true);


        // Should always be true, just sanity check
        D_ASSERT(cellsize[cell] > 1);

        int original_first_cell_length = cellsize[cell];
        int new_first_cell_length = pos - cellStartPos(cell);
        int new_second_cell_length = original_first_cell_length - new_first_cell_length;

        int new_cell_num = cellstart.size() + 1;

        SplitState ss = aq->triggerSplit(cell, new_cell_num, new_first_cell_length, new_second_cell_length);

        if(ss.hasFailed())
            return ss;

        cellsize[cell] = new_first_cell_length;
        cellstart.push_back(pos);
        cellsize.push_back(new_second_cell_length);
        markstore.marks[pos]=cellCount();
        markstore.marks_start[pos]=cellCount();

        if(cellsize[cell] == 1)
        {
            fixed.push_back(cell);
            fixed_vals.push_back(*cellStartPtr(cell));
        }

        if(cellsize[new_cell_num] == 1)
        {
            fixed.push_back(new_cell_num);
            fixed_vals.push_back(*cellStartPtr(new_cell_num));
        }

        backtrack_stack.push_back(PartitionSplit(cell, pos));
        D_SLOW_ASSERT(sanity_check());
        return ss;
    }

    void event_pushWorld()
    {
        backtrack_depths.push_back(backtrack_stack.size());
    }

    void event_popWorld()
    {
        int depth = backtrack_depths.back();
        backtrack_depths.pop_back();
        while(backtrack_stack.size() > depth)
        {
            PartitionSplit ps = backtrack_stack.back();
            backtrack_stack.pop_back();

            if(cellSize(cellCount()) == 1)
            {
                D_ASSERT(fixed.back() == cellCount());
                fixed.pop_back();
                fixed_vals.pop_back();
            }

            if(cellSize(ps.cell) == 1)
            {
                D_ASSERT(fixed.back() == ps.cell);
                fixed.pop_back();
                fixed_vals.pop_back();
            }

            D_ASSERT(markstore.marks[ps.splitpos] == cellCount());
            D_ASSERT(markstore.marks_start[ps.splitpos] == cellCount());
            markstore.marks[ps.splitpos] = 0;
            markstore.marks_start.erase(ps.splitpos);
            D_ASSERT(cellStartPos(cellCount()) == ps.splitpos);
            cellsize[ps.cell] += cellsize[cellCount()];
            cellstart.pop_back();
            cellsize.pop_back();
            D_SLOW_ASSERT(sanity_check());
        }
    }

    void swapPositions(int pos1, int pos2)
    {
        D_ASSERT(cellOfPos(pos1) == cellOfPos(pos2));
        int temp = vals[pos1];
        vals[pos1] = vals[pos2];
        vals[pos2] = temp;
        invvals[vals[pos1]] = pos1;
        invvals[vals[pos2]] = pos2;
    }

    /// Fix the inverses of a cell
    void fixCellInverses(int cell)
    {
        int start = cellStartPos(cell);
        int end = cellEndPos(cell);
        for(int i = start; i < end; ++i)
        {
            invvals[vals[i]] = i;
        }
    }


    vec1<vec1<int> > dumpCurrentPartition()
    {
        vec1<vec1<int> > partition;
        for(int i = 1; i <= cellCount(); ++i)
        {
            vec1<int> v;
            for(cellit it = cellStartPtr(i); it != cellEndPtr(i); ++it)
                v.push_back(*it);
            std::sort(v.begin(), v.end());
            partition.push_back(v);
        }
        return partition;
    }

    std::string printCurrentPartition()
    {
        vec1<vec1<int> > v = dumpCurrentPartition();
        std::ostringstream oss;
        oss << "[" << markstore.marks[1] << ": ";
        for(int i = 1; i <= n; ++i)
        {
            oss << vals[i];
            if(markstore.marks[i+1] > 0)
                oss << "|" << markstore.marks[i+1] << ": ";
            else
                oss << " ";
        }
        oss << "]";
        return oss.str();
    }
};


#endif
