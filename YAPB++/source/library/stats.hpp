#ifndef _STATS_HPP_LCDSJIOSA
#define _STATS_HPP_LCDSJIOSA

#include "vec1.hpp"
#include <ostream>
#include <iostream>
#include <map>

namespace Stats
{
struct Sort
{
    int len;
    int types;
    bool success;

    Sort(int _len, int _types, bool _success)
    : len(_len), types(_types), success(_success)
    { }

    friend std::ostream& operator<<(std::ostream& o, const Sort& s)
    {
        return o << "(" << s.len << "," << s.types << "," << s.success << ")";
    }

    friend bool operator<(const Sort& s1, const Sort& s2)
    {
        return std::make_pair(s1.len, std::make_pair(s1.types, s1.success)) <
               std::make_pair(s2.len, std::make_pair(s2.types, s2.success));
    }
};

struct Container
{
    std::map<Sort, int> sortStats;

    int node_count;
    int bad_leaves;
    int bad_internal_nodes;

    vec1<std::pair<int, int> > rBase;

    vec1<int> fixed_points;

    Container() : node_count(0), bad_leaves(0), bad_internal_nodes(0)
    { }

    void addSortStat(Sort ss)
    { sortStats[ss]++; }

    void dumpStats()
    {
        for(std::map<Sort, int>::const_iterator it = sortStats.begin(); it != sortStats.end(); ++it)
        {
            std::cerr << it->first << " : " << it->second << "\n";
        }
    }
};

inline Container& container()
{
    static Container c;
    return c;
}

void reset()
{
    container() = Container();
}

}

//#define STATS

#ifdef STATS
#define RECORD_STATS(x) Stats::container().x
#else
#define RECORD_STATS(x)
#endif


#endif
