#ifndef ALGORITHMS_HPP_YPAQW
#define ALGORITHMS_HPP_YPAQW

#include "library/vec1.hpp"
#include "library.hpp"
#include "perm.hpp"
#include <algorithm>

// Some generally useful functions

template<typename Container>
bool contains_no_repeats(Container con)
{
    std::sort(con.begin(), con.end());
    auto it = con.begin();
    while(it != con.end())
    {
        auto it2 = it;
        ++it2;
        if(it2 != con.end() && *it == *it2)
            return false;
        ++it;
    }
    return true;
}

enum MissingPoints
{ MissingPoints_Free, MissingPoints_Fixed };

inline vec1<int> partitionToList(const vec1<vec1<int> >& part, int size, MissingPoints mp)
{
    vec1<int> vec(size, 0);
    int covered = 1;
    for(int i : range1(part.size()))
    {
        for(int val : part[i])
        {
            D_ASSERT(vec[val] == 0);
            vec[val] = i;
            covered++;
        }
    }    
    // Maybe the permutation is missing some values of the end. These
    // should all be treated as defined by 'MissingPoints'
    if(mp == MissingPoints_Fixed)
    {
        for(int i : range1(vec.size()))
        {
            if(vec[i] == 0)
                vec[i] = vec.size() + 1 + i; // (is +1 required? It doesn't hurt)
        }
    }
    return vec;
}

template<MissingPoints mp>
class SparseFunction
{
    std::map<int, int> const* m;
public:

    SparseFunction(std::map<int, int> const* _m) :  m(_m)
    { }

    SparseFunction(const SparseFunction& sf) : m(sf.m)
    { }

    int operator()(int i) const
    { 
        std::map<int, int>::const_iterator it = m->find(i); 
        if(it == m->end())
        {
            if(mp == MissingPoints_Free)
                return 0;
            else
                abort();
        }
        else
            return it->second;
    }

    friend std::ostream& operator<<(std::ostream& o, const SparseFunction& sf)
    { return o << *(sf.m); }
};


inline std::map<int, int> partitionToMap(const vec1<vec1<int> >& part)
{
    std::map<int, int> m;
    int covered = 1;
    for(int i : range1(part.size()))
    {
        for(int val : part[i])
        {
            D_ASSERT(m[val] == 0);
            m[val] = i;
            covered++;
        }
    }    
    
    return m;
}


inline vec1<int> invertVecAsPermutation(const vec1<int>& v)
{
    vec1<int> ret(v.size());
    for(int i : range1(v.size()))
    {
        ret[v[i]] = i;
    }
    return ret;
}

#endif
