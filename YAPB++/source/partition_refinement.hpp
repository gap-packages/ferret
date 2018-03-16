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
        debug_out(3, "filter", "all hashes equal: " + toString(first_val));
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
    debug_out(3, "filter",  "Succeeded at: " << cell << ", " << cellsplits << " ( " << (cellEnd - cellBegin) << ", " << cellBegin << ", " << cellEnd << ", " << ps->cellCount() << ")\n");
    return se;
}

#define STABCHAIN_DEBUG_INFO cell << ", " << " ( " << ps->cellSize(cell) << "," << (cellEnd - cellBegin) << ", " << cellBegin << ", " << cellEnd << ", " << ps->cellCount() << ")"


template<typename F>
inline SplitState filterCellByFunction_withSortData(PartitionStack* ps, int cell, F f, const SortEvent& se)
{
    debug_out(3, "filter",  "Start filter");

    D_ASSERT(se.hash_starts.size() > 1);

    if(!indirect_data_sorter(cell, ps, f, se))
        return SplitState(false);

    ps->fixCellInverses(cell);

    return SplitState(true);
}

template<typename F>
bool validateFixedCell(PartitionStack* ps, int cell, int hash, F f)
{
    debug_out(3, "filter", "validating fixed cell by hash " << hash);
    for(int val : ps->cellRange(cell))
    {
        if((int)f(val) != hash)
        {
            debug_out(3, "filter", "error: found " << f(val) << " for " << val);
            return false;
        }
    }
    RECORD_STATS(addSortStat(Stats::Sort(ps->cellSize(cell), 1, true)));
    return true;
}

template<typename F>
SplitState filterPartitionStackByFunction_withSortData(PartitionStack* ps, F f)
{
	PartitionEvent& pe = ps->getAbstractQueue()->getPartitionEvent();
	int len = 0;
	for(auto it = pe.order.begin(); it != pe.order.end(); ++it)
	{
	    len++;
	    if(it->change)
	    {
	        int cell = pe.change_cells[it->index].first;
	        bool sorter = indirect_data_sorter(cell, ps, f, pe.change_cells[it->index].second);
	        ps->fixCellInverses(cell);
	        if(!sorter)
	        {
                debug_out(3, "filterPSBF_WSD", "failed " << cell);
	            pe.order.promote(it);
	            return SplitState(false);
	        }
	    }
	    else
	    {
	        if(!validateFixedCell(ps, pe.no_change_cells[it->index].first, pe.no_change_cells[it->index].second, f))
	        {
	            pe.order.promote(it);
                debug_out(3, "filterPSBF_WSD", "failed validate fixed cell");
	            return SplitState(false);
	        }
	    }

	}

	// Finally, do the splits
	for(int i : range1(pe.change_cells.size()))
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
void filterCell(PartitionStack* ps, F f, int i, PartitionEvent* pe)
{
    if(ps->cellSize(i) > 1 || TUNABLE_FILTER_SINGLE_CELLS)
    {
        SortEvent se = filterCellByFunction_noSortData(ps, i, f);
        if(se.noSplits())
        {
            pe->no_change_cells.push_back(std::make_pair(i, se.hash_starts[1].hashVal));
        }
        else
        {
            pe->change_cells.push_back(std::make_pair(i, std::move(se)));
        }
    }
}
template<typename F>
SplitState filterPartitionStackByFunction_noSortData(PartitionStack* ps, F f)
{
    PartitionEvent pe;
    int cellCount = ps->cellCount();
    for(int i : range1(cellCount))
    {
        filterCell(ps, f, i, &pe);
    }
    pe.finalise();
    ps->getAbstractQueue()->addPartitionEvent(std::move(pe));
    return SplitState(true);
}

template<typename F, typename Cells>
SplitState filterPartitionStackByFunctionWithCells_noSortData(PartitionStack* ps, F f, const Cells& cells)
{
    PartitionEvent pe;
    for(int c : cells)
        filterCell(ps, f, c, &pe);
    pe.finalise();
    ps->getAbstractQueue()->addPartitionEvent(std::move(pe));
    return SplitState(true);
}

template<typename F>
SplitState filterPartitionStackByFunction(PartitionStack* ps, F f)
{
    debug_out(3, "filterByFunction", "prestate " << ps->printCurrentPartition());
    SplitState ret(false);
    if(ps->getAbstractQueue()->hasSortData())
      ret = filterPartitionStackByFunction_withSortData(ps, f);
    else
      ret = filterPartitionStackByFunction_noSortData(ps, f);
    debug_out(3, "filterByFunction", "poststate " << ps->printCurrentPartition() << ":" << ret);
    return ret;
}

template<typename F, typename Cells>
SplitState filterPartitionStackByFunctionWithCells(PartitionStack* ps, F f, const Cells& cells)
{
    debug_out(3, "filterByFunction", "prestate " << ps->printCurrentPartition());
    SplitState ret(false);
    if(ps->getAbstractQueue()->hasSortData())
    {
      // TODO : Use cells
      ret = filterPartitionStackByFunction_withSortData(ps, f);
    }
    else
      ret = filterPartitionStackByFunctionWithCells_noSortData(ps, f, cells);
    debug_out(3, "filterByFunctionWC", "poststate " << ps->printCurrentPartition() << ":" << ret);
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
    debug_out(3, "filterUnFun", "prestate " << ps->printCurrentPartition());
    int cellCount = ps->cellCount();
    // first of all, we need to try to distinguish as many values of F as possible.

    typedef typename std::remove_reference<decltype(f(0))>::type result_type ;
    std::map<result_type, HashType> full_hash;

    for(int i : range1(cellCount))
    {
    	typedef std::map<result_type, unsigned> map_type;
    	map_type count_map;
    	for(int x : ps->cellRange(i))
    	{
    		count_map[f(x)]++;
    	}

    	for(const auto& m : count_map)
    	{
    		full_hash[m.first] = hash_combine(full_hash[m.first], i, m.second);
    	}
    }

    debug_out(3, "filter", "Hash:" << full_hash);
    debug_out(3, "filter", "Function:" << f);
    SplitState ret = filterPartitionStackByFunction(ps, [&](auto i){return CheckedMap(full_hash, f(i));});
    debug_out(3, "filterUnFun", "poststate " << ps->printCurrentPartition());
    return ret;
}

template<typename F>
SplitState filterPartitionStackByUnorderedListFunction(PartitionStack* ps, F f)
{
    debug_out(3, "filterUnListFun", "prestate " << ps->printCurrentPartition());
    int cellCount = ps->cellCount();
    // first of all, we need to try to distinguish as many values of F as possible.
    typedef typename std::remove_reference<decltype(f(0))>::type result_type ;


    std::map<typename result_type::value_type, HashType> full_hash;

    for(int i : range1(cellCount))
    {
        typedef std::map<typename result_type::value_type, unsigned> map_type;
        map_type count_map;
        for(int val : ps->cellRange(i))
        {
            for(const auto& val2 : f(val))
                count_map[val2]++;
        }

        for(const auto& val : count_map)
        {
            full_hash[val.first] = hash_combine(full_hash[val.first], i, val.second);
        }
    }

    SplitState ret = filterPartitionStackByFunction(ps, IndirectVecCollapseFunction([&](auto i) -> auto& { return CheckedMap(full_hash, i); }, f));
    debug_out(3, "filterUnListFun", "poststate " << ps->printCurrentPartition());
    return ret;
}

template<typename F>
SplitState filterPartitionStackBySetTupleFunction(PartitionStack* ps, F f)
{
    debug_out(3, "filterSetTuple", "prestate " << ps->printCurrentPartition());
    int cellCount = ps->cellCount();

    // first of all, we need to try to distinguish as many sets in F as possible.
    std::map<int, HashType> full_hash;

    for(int i : range1(cellCount))
    {
        typedef std::map<int, HashType> map_type;
        map_type count_map;
        for(int val : ps->cellRange(i))
        {
            for(const auto& val2 : f(val))
                count_map[val2.first]+=quick_hash(val2.second);
        }

        for(const auto& val : count_map)
        {
            full_hash[val.first] = hash_combine(full_hash[val.first], i, val.second);
        }
    }

    vec1<int> hash_val(ps->domainSize());

    for(auto v : range1(ps->domainSize())) {
        HashType hash = 0;
        for(auto& val2 : f(v)) {
            hash += hash_combine(full_hash[val2.first], val2.second);
        }
        hash_val[v] = hash;
    }


    SplitState ret = filterPartitionStackByFunction(ps, [&](auto i) -> auto& { return hash_val[i]; });
    debug_out(3, "filterSetTuple", "poststate " << ps->printCurrentPartition());
    return ret;
}


#endif
