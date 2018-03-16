#include <algorithm>
#include <iostream>
#include "library/vec1.hpp"
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
	static thread_local vec1<vec1<int> > threaded_v;

    vec1<vec1<int> >* v = &threaded_v;
	debug_out(3, "sort", "start");
	v->resize(sd.hash_starts.size());

	for(int i : range1(sd.hash_starts.size()))
	{
    (void)i;
		D_ASSERT((*v)[i].empty());
	}

	typedef typename PartitionStack::cellit it_type;
	it_type cellStart = ps->cellStartPtr(cell);
	it_type cellEnd = ps->cellEndPtr(cell);

	for(it_type ptr = cellStart; ptr < cellEnd; ++ptr)
	{
		HashType hash = f(*ptr);
		vec1<HashInvPosition>::const_iterator pos = std::lower_bound(sd.Hash_inv_pos.begin(),
                                                                     sd.Hash_inv_pos.end(), hash, compareHash);
		if(pos == sd.Hash_inv_pos.end() || pos->hashVal != hash)
		{
			for(int i : range1(v->size()))
				(*v)[i].clear();
			return false;
		}
//        int location = pos->pos;
		int location = pos - sd.Hash_inv_pos.begin() + 1;
		if((*v)[location].size() == sd.hash_starts[sd.Hash_inv_pos[location].pos].count)
		{
			for(int i : range1(v->size()))
				(*v)[i].clear();
			return false;
		}
		(*v)[location].push_back(*ptr);
	}

	// Sorting succeeded! Now lets put everything back.

	for(int i = sd.hash_starts.size(); i >= 1; --i)
	{
        int pos = sd.Hash_inv_pos[i].pos;
    	D_ASSERT(sd.Hash_inv_pos[i].hashVal == sd.hash_starts[pos].hashVal);
		D_ASSERT((*v)[i].size() == sd.hash_starts[pos].count);

		std::copy((*v)[i].begin(), (*v)[i].end(), ps->valPtr(sd.hash_starts[pos].startPos));

		debug_out(3, "sort", "clear: " << i);
		(*v)[i].clear();
	}

	debug_out(3, "sort", "end");
	return true;
}

template<typename PartitionStack, typename SortFun>
bool indirect_data_sorter(int cell, PartitionStack* ps, SortFun f, const SortEvent& sd)
{
	bool b = indirect_data_sorter_impl(cell, ps, f, sd);

	RECORD_STATS(addSortStat(Stats::Sort(ps->cellSize(cell), sd.Hash_inv_pos.size(), b)));

	return b;
}
