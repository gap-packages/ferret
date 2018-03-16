#ifndef _SOL_STORE_QPCHEK
#define _SOL_STORE_QPCHEK

#include "library/vec1.hpp"
#include "library/library.hpp"
#include "rbase/rbase.hpp"
#include "library/perm.hpp"
#include "partition_refinement.hpp"

class SolutionStore
{
    RBase* rb;
    // List of solutions
    vec1<Permutation> permutations;
    // List which tells us that solution i was generated when trying to map .first to .second
    // this is useful for building a stabilizer chain later.
    vec1<std::pair<int, int> > permutations_from;
    
    vec1<int> orbit_mins;

    // This is here to allow different functions
    // later.
    bool comparison(int i, int j) const
    { return IndirectSorter([&](auto i) -> auto& { return (rb->inv_value_ordering)[i]; })(i, j); }

public:
    SolutionStore(RBase* _rb) : rb(_rb), orbit_mins(rb->initial_permstack->domainSize(), -1)
    { }

    bool isMinimum(int i) const
    { 
        bool isMin = (orbit_mins[i] == -1);
        if(isMin)
        { debug_out(1, "SolutionStore", "NotMin: " << i); }
        return orbit_mins[i] == -1; 
    }

    vec1<int> getMins() const
    {
        vec1<int> minimums;
        for(int i : range1(orbit_mins.size()))
        {
            if(isMinimum(i))
                minimums.push_back(i);
        }
        return minimums;
    }

    int walkToMinimum(int pos) const
    {
        while(orbit_mins[pos] != -1)
        {
            D_ASSERT(comparison(orbit_mins[pos], pos));
            pos = orbit_mins[pos];
        }
        return pos;
    }

    void update_orbit_mins(int min_val, int pos)
    {
        D_ASSERT(!comparison(pos, min_val));
        if(min_val != pos)
            orbit_mins[pos] = min_val;
    }

    void addSolution(const Permutation& sol)
    {
        permutations.push_back(sol);
        D_ASSERT(sol.size() == orbit_mins.size());
        debug_out(3, "SS", "Old orbit_mins:" << orbit_mins);
        for(int i : range1(sol.size()))
        {
            if(sol[i] != i)
            {
                int val1 = walkToMinimum(i);
                int val2 = walkToMinimum(sol[i]);
                int orbit_min = -1;
                if(comparison(val1, val2))
                    orbit_min = val1;
                else
                    orbit_min = val2;

                update_orbit_mins(orbit_min, val1);
                update_orbit_mins(orbit_min, val2);
                update_orbit_mins(orbit_min, i);
                update_orbit_mins(orbit_min, sol[i]);
            }
        }
        debug_out(1, "SS", "Solution found");
        debug_out(3, "SS", "Sol:" << sol);
        debug_out(3, "SS", "New orbit_mins:" << orbit_mins);
    }

    void markLastSolutionFrom(int from, int to)
    {
        D_ASSERT(permutations.size() == permutations_from.size() + 1);
        permutations_from.push_back(std::make_pair(from, to));
    }
    
    const vec1<Permutation>& sols() const
    { return permutations; }
    
    const vec1<std::pair<int,int> >& solsmap() const
    { return permutations_from; }
    
    const RBase* getRBase() const
    { return rb; }

    friend std::ostream& operator<<(std::ostream& os, const SolutionStore& ss)
    { return os << ss.permutations; }
};
#endif
