#ifndef MONOSET_HPP_APQG
#define MONOSET_HPP_APQG

#include <vector>

class MonoSet
{
	std::vector<bool> hit;
	std::vector<int> members;
public:
	//+1 to handle 0-based and 1-based containers
	MonoSet(int size) : hit(size+1, false)
	{ }

	DEFAULT_MOVE_COPY_CONST_ASSIGN(MonoSet);

	void add(int i)
	{
		if(!hit[i])
		{
			hit[i] = true;
			members.push_back(i);
		}
	}

	bool check(int i) const
	{
		return hit[i];
	}

	const std::vector<int>& getMembers() const
	{ return members; }

	std::vector<int>::const_iterator begin() const
	{ return members.begin(); }

	std::vector<int>::const_iterator end() const
	{ return members.end(); }

    typedef std::vector<int>::const_iterator iterator;
    typedef std::vector<int>::const_iterator const_iterator;
    
	int size() const
	{ return members.size(); }

	void clear()
	{
		for(unsigned i = 0; i < members.size(); ++i)
			hit[members[i]] = false;
		members.clear();
	}

};

#endif
