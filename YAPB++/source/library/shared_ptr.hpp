#ifndef _SHARED_PTR_HPP_CAJ
#define _SHARED_PTR_HPP_CAJ

#include "library.hpp"


class shared_ptr_base
{
	int count;
public:
	shared_ptr_base() : count(1)
	{ }

	void increment_counter()
	{ count++; }

	int decrement_counter()
	{
		D_ASSERT(count > 0);
		count--;
		return count;
	}

	~shared_ptr_base()
	{ D_ASSERT(count == 0); }
};


#endif
