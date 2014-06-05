#ifndef PART_REFINE_FDSJIFDS
#define PART_REFINE_FDSJIFDS

#include "partition_stack.hpp"
#include "library/mappers.hpp"
#include "library/hash.hpp"
#include "tunables.hpp"

template<typename F>
SortEvent filterCellByFunction_noSortData(PartitionStack* ps, int cell, F f)
{
    // Start at the end, because then the cell we pass to
    // split remains the same.

    int cellBegin = ps->cellStartPos(cell);
    int cellEnd = ps->cellEndPos(cell);


    // Cheap pass to check if all cells have the same value
    HashType first_val = f(ps->val(cellBegin));

    bool allequal = true;

    for(int pos = cellBegin + 1; pos < cellEnd && allequal; ++pos)
    {
        if(first_val != (HashType)f(ps->val(pos)))
            allequal = false;
    }

    if(allequal)
    {
        SortEvent se(cellBegin, cellEnd);
        se.addHashStart(f(ps->val(cellBegin)), cellBegin);
        se.finalise();
        return se;
    }

    std::sort(ps->cellStartPtr(cell), ps->cellEndPtr(cell), IndirectSorter(f));
    ps->fixCellInverses(cell);


    int cellsplits = 0;

    SortEvent se(cellBegin, cellEnd);

    for(int pos = cellEnd - 2; pos >= cellBegin; --pos)
    {
        if(f(ps->val(pos)) != f(ps->val(pos+1)))
        {
            se.addHashStart(f(ps->val(pos+1)),pos+1);
            cellsplits++;
            if(ps->split(cell, pos+1).hasFailed())
                abort();
        }
    }

    // Have to have information about the first cell too!
    se.addHashStart(f(ps->val(cellBegin)), cellBegin);

    se.finalise();

    D_ASSERT(cellsplits > 0);
    debug_out(0, "filter",  "Succeeded at: " << cell << ", " << cellsplits << " ( " << (cellEnd - cellBegin) << ", " << cellBegin << ", " << cellEnd << ", " << ps->cellCount() << ")\n");
    return se;
}

#define STABCHAIN_DEBUG_INFO cell << ", " << " ( " << ps->cellSize(cell) << "," << (cellEnd - cellBegin) << ", " << cellBegin << ", " << cellEnd << ", " << ps->cellCount() << ")"


template<typename F>
inline SplitState filterCellByFunction_withSortData(PartitionStack* ps, int cell, F f, const SortEvent& se)
{
    debug_out(0, "filter",  "Start filter");

    D_ASSERT(se.hash_starts.size() > 1);

    if(!indirect_data_sorter(cell, ps, f, se))
        return SplitState(false);

    ps->fixCellInverses(cell);

    return SplitState(true);
}

template<typename F>
bool validateFixedCell(PartitionStack* ps, int cell, int hash, F f)
{
    debug_out(0, "filter", "validating fixed cell by hash " << hash);
    typedef PartitionStack::cellit it_type;
    it_type end = ps->cellEndPtr(cell);
    for(it_type it = ps->cellStartPtr(cell); it != end; ++it)
    {
        if((int)f(*it) != hash)
            return false;
    }
    RECORD_STATS(addSortStat(Stats::Sort(ps->cellSize(cell), 1, true)));
    return true;
}

template<typename F>
SplitState filterPartitionStackByFunction_withSortData(PartitionStack* ps, F f)
{
	PartitionEvent& pe = ps->getAbstractQueue()->getPartitionEvent();
	typedef PartitionEvent::PromotableList::iterator it_type;
	int len = 0;
	for(it_type it = pe.order.begin(); it != pe.order.end(); ++it)
	{
	    len++;
	    if(it->change)
	    {
	        int cell = pe.change_cells[it->index].first;
	        bool sorter = indirect_data_sorter(cell, ps, f, pe.change_cells[it->index].second);
	        ps->fixCellInverses(cell);
	        if(!sorter)
	        {
	            pe.order.promote(it);
	            return SplitState(false);
	        }
	    }
	    else
	    {
	        if(!validateFixedCell(ps, pe.no_change_cells[it->index].first, pe.no_change_cells[it->index].second, f))
	        {
	            pe.order.promote(it);
	            return SplitState(false);
	        }
	    }

	}

	// Finally, do the splits
	for(int i = 1; i <= pe.change_cells.size(); ++i)
	{
	    int cell = pe.change_cells[i].first;
	    const SortEvent& se = pe.change_cells[i].second;
	    for(int j = 1; j < se.hash_starts.size(); ++j)
	    {
	        if(ps->split(cell, se.hash_starts[j].startPos).hasFailed())
	        { abort(); }
	    }
	}
	return SplitState(true);
}

template<typename F>
SplitState filterPartitionStackByFunction_noSortData(PartitionStack* ps, F f)
{
    PartitionEvent pe;
    int cellCount = ps->cellCount();
    for(int i = 1; i <= cellCount; ++i)
    {
        if(ps->cellSize(i) > 1 || TUNABLE_FILTER_SINGLE_CELLS)
        {
            SortEvent se = filterCellByFunction_noSortData(ps, i, f);
            if(se.noSplits())
            {
                pe.no_change_cells.push_back(std::make_pair(i, se.hash_starts[1].hashVal));
            }
            else
            {
                pe.change_cells.push_back(std::make_pair(i, std::move(se)));
            }
        }
    }
    pe.finalise();
    ps->getAbstractQueue()->addPartitionEvent(std::move(pe));
    return SplitState(true);
}

template<typename F>
SplitState filterPartitionStackByFunction(PartitionStack* ps, F f)
{
    debug_out(0, "filterByFunction", "prestate " << ps->printCurrentPartition());
    SplitState ret(false);
    if(ps->getAbstractQueue()->hasSortData())
      ret = filterPartitionStackByFunction_withSortData(ps, f);
    else
      ret = filterPartitionStackByFunction_noSortData(ps, f);
    debug_out(0, "filterByFunction", "poststate " << ps->printCurrentPartition());
    return ret;
}

template<typename F>
SplitState filterCellByUnorderedFunction(PartitionStack* ps, int cell, F f)
{
    // First, gather up counts.
    std::map<int, int> m;

    int cellBegin = ps->cellStartPos(cell);
    int cellEnd = ps->cellEndPos(cell);

    for(int pos = cellBegin; pos < cellEnd; ++pos)
    {
        m[ps->val(pos)]++;
    }

    abort();

    PartitionStack::cellit cellPtrBegin = ps->cellStartPtr(cell);
    PartitionStack::cellit cellPtrEnd = ps->cellEndPtr(cell);
    std::sort(cellPtrBegin, cellPtrEnd, IndirectSorter(f));
    ps->fixCellInverses(cell);


    // Start at the end, because then the cell we pass to
    // split remains the same.


    for(int pos = cellEnd - 2; pos >= cellBegin; --pos)
    {
        if(f(ps->val(pos)) != f(ps->val(pos+1)))
        {
            if(ps->split(cell, pos+1).hasFailed())
                return SplitState(false);
        }
    }
    return SplitState(true);
}

template<typename F>
SplitState filterPartitionStackByUnorderedFunction(PartitionStack* ps, F f)
{
    debug_out(0, "filterUnFun", "prestate " << ps->printCurrentPartition());
    int cellCount = ps->cellCount();
    // first of all, we need to try to distinguish as many values of F as possible.

    std::map<typename F::result_type, HashType> full_hash;

    for(int i = 1; i <= cellCount; ++i)
    {
    	typedef std::map<typename F::result_type, unsigned> map_type;
    	map_type count_map;
    	typedef PartitionStack::cellit cell_it;
    	for(cell_it it = ps->cellStartPtr(i); it != ps->cellEndPtr(i); ++it)
    	{
    		count_map[f(*it)]++;
    	}

    	for(typename map_type::iterator it = count_map.begin(); it != count_map.end(); ++it)
    	{
    		full_hash[it->first] = hash_combine(full_hash[it->first], i, it->second);
    	}
    }

    debug_out(0, "filter", "Hash:" << full_hash);
    debug_out(0, "filter", "Function:" << f);
    SplitState ret = filterPartitionStackByFunction(ps, IndirectFunction(MapToFunction(&full_hash), f));
    debug_out(0, "filterUnFun", "poststate " << ps->printCurrentPartition());
    return ret;
}

template<typename F>
SplitState filterPartitionStackByUnorderedListFunction(PartitionStack* ps, F f)
{
    debug_out(0, "filterUnListFun", "prestate " << ps->printCurrentPartition());
    int cellCount = ps->cellCount();
    // first of all, we need to try to distinguish as many values of F as possible.

    std::map<typename F::result_type::value_type, HashType> full_hash;

    for(int i = 1; i <= cellCount; ++i)
    {
        typedef std::map<typename F::result_type::value_type, unsigned> map_type;
        map_type count_map;
        typedef PartitionStack::cellit cell_it;
        for(cell_it it = ps->cellStartPtr(i); it != ps->cellEndPtr(i); ++it)
        {
            typedef typename F::result_type con_type;
            const con_type& res = f(*it);
            for(typename con_type::const_iterator it2 = res.begin(); it2 != res.end(); ++it2)
                count_map[*it2]++;
        }

        for(typename map_type::iterator it = count_map.begin(); it != count_map.end(); ++it)
        {
            full_hash[it->first] = hash_combine(full_hash[it->first], i, it->second);
        }
    }

    SplitState ret = filterPartitionStackByFunction(ps, IndirectVecCollapseFunction(MapToFunction(&full_hash), f));
    debug_out(0, "filterUnListFun", "poststate " << ps->printCurrentPartition());
    return ret;
}


#endif
