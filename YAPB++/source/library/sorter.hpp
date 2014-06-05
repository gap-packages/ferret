#include <algorithm>
#include <iostream>
#include "vec1.hpp"
#include "library.hpp"
#include "sort_events.hpp"
#include "stats.hpp"

template<typename Fun>
struct IndirectSorter_impl
{
    Fun f;
    IndirectSorter_impl(Fun _f) : f(_f)
    { }

    template<typename T>
    bool operator()(const T& lhs, const T& rhs)
    { return f(lhs) < f(rhs); }
};

template<typename Fun>
IndirectSorter_impl<Fun> IndirectSorter(const Fun& f)
{ return IndirectSorter_impl<Fun>(f); }

template<typename Fun>
struct ReverseSorter_impl
{
    Fun f;
    ReverseSorter_impl(Fun _f) : f(_f)
    { }

    template<typename T>
    bool operator()(const T& lhs, const T& rhs)
    { return f(rhs, lhs); } // <- Yes, this is the wrong way around, that is the point!
};

template<typename Fun>
ReverseSorter_impl<Fun> ReverseSorter(const Fun& f)
{ return ReverseSorter_impl<Fun>(f); }


template<typename PartitionStack, typename SortFun>
bool indirect_data_sorter_impl(int cell, PartitionStack* ps, SortFun f, const SortEvent& sd)
{
	static vec1<vec1<int> > v;
	debug_out(0, "sort", "start");
	v.resize(sd.hash_starts.size());

	for(int i = 1; i <= sd.hash_starts.size(); ++i)
	{
		D_ASSERT(v[i].empty());
	}

	typedef typename PartitionStack::cellit it_type;
	it_type cellStart = ps->cellStartPtr(cell);
	it_type cellEnd = ps->cellEndPtr(cell);

	for(it_type ptr = cellStart; ptr < cellEnd; ++ptr)
	{
		int hash = f(*ptr);
		auto pos = std::lower_bound(sd.hash_counts.begin(), sd.hash_counts.end(), hash, compareHash);
		if(pos == sd.hash_counts.end() || pos->hashVal != hash)
		{
			for(int i = 1; i <= v.size(); ++i)
				v[i].clear();
			return false;
		}
		int location = pos - sd.hash_counts.begin() + 1;
		if(v[location].size() == sd.hash_counts[location].count)
		{
			for(int i = 1; i <= v.size(); ++i)
				v[i].clear();
			return false;
		}
		v[location].push_back(*ptr);
	}

	// Sorting succeeded! Now lets put everything back.

	for(int i = sd.hash_starts.size(); i >= 1; --i)
	{
		D_ASSERT(sd.hash_counts[i].hashVal == sd.hash_starts[sd.hash_order[i]].hashVal);
		D_ASSERT(v[i].size() == sd.hash_counts[i].count);

		std::copy(v[i].begin(), v[i].end(), ps->valPtr(sd.hash_starts[sd.hash_order[i]].startPos));

		debug_out(0, "sort", "clear: " << i);
		v[i].clear();
	}

	debug_out(0, "sort", "end");
	return true;
}

template<typename PartitionStack, typename SortFun>
bool indirect_data_sorter(int cell, PartitionStack* ps, SortFun f, const SortEvent& sd)
{
	bool b = indirect_data_sorter_impl(cell, ps, f, sd);

	RECORD_STATS(addSortStat(Stats::Sort(ps->cellSize(cell), sd.hash_counts.size(), b)));

	return b;
}
