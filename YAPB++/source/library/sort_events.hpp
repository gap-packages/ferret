#ifndef _SORT_EVENT_CDHUCDS
#define _SORT_EVENT_CDHUCDS
#include "library.hpp"
#include "vec1.hpp"
#include "promotable_list.hpp"

typedef int HashType;

struct HashStart
{
	HashType hashVal;
	int startPos;
	int count;

	HashStart(int _hv, int _sp) : hashVal(_hv), startPos(_sp)
	{ }
};

struct HashCount
{
	HashType hashVal;
	int pos;

	HashCount(int _hv, int _pos) : hashVal(_hv), pos(_pos)
	{  }
};

inline bool operator<(const HashCount& lhs, const HashCount& rhs)
{ return std::make_pair(lhs.hashVal, lhs.pos) < std::make_pair(rhs.hashVal, rhs.pos); }

inline bool compareHash(const HashCount& lhs, int rhs)
{ return lhs.hashVal < rhs; }

struct SortEvent
{
	int cellBegin, cellEnd;

	// note these are generated in reverse order, as this is how we split
	vec1<HashStart> hash_starts;

	// This is hash_starts ordered by hash value, with size of each block
	vec1<HashCount> hash_counts;

public:
	SortEvent(int _cellBegin, int _cellEnd) : cellBegin(_cellBegin), cellEnd(_cellEnd)
	{}

    DEFAULT_MOVE_COPY_CONST_ASSIGN(SortEvent);
public:

	bool noSplits() const
	{ return hash_starts.size() == 1; }

	void addHashStart(int hash, int pos)
	{
		D_ASSERT(hash_counts.empty());
		D_ASSERT(pos >= cellBegin && pos < cellEnd);
		hash_starts.push_back(HashStart(hash, pos));
	}

	void finalise()
	{
		D_ASSERT(hash_counts.empty());
		int hashes = hash_starts.size();
		hash_counts.reserve(hashes + 1);

		// We want to know the order the hashes occur in
		for(int i = 2; i <= hashes; ++i)
		{
			HashCount hc(hash_starts[i].hashVal, i);
			hash_counts.push_back(hc);
			hash_starts[i].count = hash_starts[i-1].startPos - hash_starts[i].startPos;
		}
		hash_counts.push_back(HashCount(hash_starts[1].hashVal, 1));
		hash_starts[1].count = cellEnd - hash_starts[1].startPos;

		std::sort(hash_counts.begin(), hash_counts.end());
	}
};

template<typename T>
struct ChangeSorter
{
	T* ptr;

	ChangeSorter(T* _ptr) : ptr(_ptr) {}

	bool operator()(int lhs, int rhs)
	{
		int lhs_size = ptr->change_cells[lhs].second.cellEnd - ptr->change_cells[lhs].second.cellBegin;
		int rhs_size = ptr->change_cells[rhs].second.cellEnd - ptr->change_cells[rhs].second.cellBegin;
		return lhs_size < rhs_size;
	}
};

struct PartitionEvent
{
	// pair is: cell, callvalue under function.
	vec1<std::pair<int, int> > no_change_cells;
	// pair is: cell, SortEvent for cell.
	vec1<std::pair<int, SortEvent> > change_cells;

	struct EventOrder
	{
		int index;
		bool change;

		EventOrder(int _index, bool _change) : index(_index), change(_change)
		{ }
	};

	typedef OneMovePromotableList<EventOrder> PromotableList;
	PromotableList order;

	PartitionEvent() = default;
	DEFAULT_MOVE_COPY_CONST_ASSIGN(PartitionEvent);

	void finalise()
	{
		D_ASSERT(order.empty());

		vec1<int> changing_cells;

		for(int i = 1; i <= change_cells.size(); ++i)
			changing_cells.push_back(i);

		std::sort(changing_cells.begin(), changing_cells.end(), ChangeSorter<PartitionEvent>(this));

		for(int i = 1; i <= change_cells.size(); ++i)
			order.push_back(EventOrder(changing_cells[i], true));

		for(int i = 1; i <= no_change_cells.size(); ++i)
			order.push_back(EventOrder(i, false));

	}

};

#endif
