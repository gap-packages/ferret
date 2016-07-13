#ifndef PROMOTABLE_LIST_HPP_FDFCDSFDS
#define PROMOTABLE_LIST_HPP_FDFCDSFDS

#include "library/vec1.hpp"
#include <list>

// A promotable list acts like a normal C++ container,
// except we add the method 'promote(iterator)', which can
// move a member higher up the list (in a container-defined way).

template<typename T>
class NullPromotableList
{
	vec1<T> l;
public:
	typedef typename vec1<T>::iterator iterator;

	iterator begin()
	{ return l.begin(); }

	iterator end()
	{ return l.end(); }

	void promote(iterator)
	{ }

	bool empty() const
	{ return l.empty(); }

	void push_back(const T& t)
	{ l.push_back(t); }
};

template<typename T>
class OneMovePromotableList
{
	vec1<T> l;
public:
	typedef typename vec1<T>::iterator iterator;

	iterator begin()
	{ return l.begin(); }

	iterator end()
	{ return l.end(); }

	void promote(iterator it)
	{
		if(it != begin())
			std::iter_swap(it, it - 1);
	}

	bool empty() const
	{ return l.empty(); }

	void push_back(const T& t)
	{ l.push_back(t); }

};

template<typename T>
class MoveToFrontPromotableList
{
	std::list<T> l;
public:
	typedef typename std::list<T>::iterator iterator;

	iterator begin()
	{ return l.begin(); }

	iterator end()
	{ return l.end(); }

	void promote(iterator it)
	{ l.splice(l.begin(), l, it); }

	bool empty() const
	{ return l.empty(); }

	void push_back(const T& t)
	{ l.push_back(t); }

};


#endif