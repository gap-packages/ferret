#ifndef _SORT_EVENT_CDHUCDS
#define _SORT_EVENT_CDHUCDS
#include "library.hpp"
#include "library/vec1.hpp"
#include "promotable_list.hpp"

typedef u_int32_t HashType;

struct HashStart
{
	HashType hashVal;
	int startPos;
	int count;

	HashStart(int _hv, int _sp) : hashVal(_hv), startPos(_sp)
	{ }
};

struct HashInvPosition
{
	HashType hashVal;
	int pos;

	HashInvPosition(int _hv, int _pos) : hashVal(_hv), pos(_pos)
	{  }
};

inline bool operator<(const HashInvPosition& lhs, const HashInvPosition& rhs)
{ return std::make_pair(lhs.hashVal, lhs.pos) < std::make_pair(rhs.hashVal, rhs.pos); }

inline bool compareHash(const HashInvPosition& lhs, HashType rhs)
{ return lhs.hashVal < rhs; }

struct SortEvent
{
	int cellBegin, cellEnd;

	// note these are generated in reverse order, as this is how we split
	vec1<HashStart> hash_starts;

	// This is hash_starts ordered by hash value, with size of each block
	vec1<HashInvPosition> Hash_inv_pos;

public:
	SortEvent(int _cellBegin, int _cellEnd) : cellBegin(_cellBegin), cellEnd(_cellEnd)
	{}

    DEFAULT_MOVE_COPY_CONST_ASSIGN(SortEvent);
public:

	bool noSplits() const
	{ return hash_starts.size() == 1; }

	void addHashStart(int hash, int pos)
	{
		D_ASSERT(Hash_inv_pos.empty());
		D_ASSERT(pos >= cellBegin && pos < cellEnd);
		hash_starts.push_back(HashStart(hash, pos));
	}

	void finalise()
	{
		D_ASSERT(Hash_inv_pos.empty());
		int hashes = hash_starts.size();
		Hash_inv_pos.reserve(hashes);

		// We want to know the order the hashes occur in
		for(int i = 2; i <= hashes; ++i)
		{
			Hash_inv_pos.push_back(HashInvPosition(hash_starts[i].hashVal, i));
			hash_starts[i].count = hash_starts[i-1].startPos - hash_starts[i].startPos;
		}
		Hash_inv_pos.push_back(HashInvPosition(hash_starts[1].hashVal, 1));
		hash_starts[1].count = cellEnd - hash_starts[1].startPos;

		std::sort(Hash_inv_pos.begin(), Hash_inv_pos.end());
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

	PartitionEvent() {};
	DEFAULT_MOVE_COPY_CONST_ASSIGN(PartitionEvent);

	void finalise()
	{
		D_ASSERT(order.empty());

		vec1<int> changing_cells;

		for(int i : range1(change_cells.size()))
			changing_cells.push_back(i);

		std::sort(changing_cells.begin(), changing_cells.end(), ChangeSorter<PartitionEvent>(this));

		for(int i : range1(change_cells.size()))
			order.push_back(EventOrder(changing_cells[i], true));

		for(int i : range1(no_change_cells.size()))
			order.push_back(EventOrder(i, false));

	}

};

#endif
