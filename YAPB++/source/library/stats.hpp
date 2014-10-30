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

enum ConstraintType {
    CON_ListStab,
    CON_OverlappingSetSetStab,
    CON_PermGroup,
    CON_SetSetStab,
    CON_SlowGraph,
    CON_StabChain_PermGroup,
    CON_END
};

struct Container
{
    std::map<Sort, int> sortStats;
    int node_count;
    int bad_leaves;
    int bad_internal_nodes;

    std::vector<int> constraint_invokes;

    vec1<std::pair<int, int> > rBase_fixed_points;

    vec1<int> rBase_value_ordering;

    Container() : node_count(0), bad_leaves(0), bad_internal_nodes(0),
                  constraint_invokes(CON_END, 0)
    { }

    std::map<std::string, int> getConstraintCalls() const
    {
        std::string names[] = {
            "ListStab", "OverlappingSetSetStab", "PermGroup",
            "SetSetStab", "SlowGraph", "StabChain_PermGroup"};

        std::map<std::string, int> ret;
        for(int i = 0; i < CON_END; ++i)
        {
            ret[names[i]] = constraint_invokes[i];
        }
        return ret;
    }

    void addConstraintCall(ConstraintType t)
    { constraint_invokes[t]++; }

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

inline void reset()
{
    container() = Container();
}

inline
void ConstraintInvoke(Stats::ConstraintType t)
{ Stats::container().addConstraintCall(t); }

}

//#define STATS


#ifdef STATS
#define RECORD_STATS(x) Stats::container().x
#else
#define RECORD_STATS(x)
#endif


#endif
