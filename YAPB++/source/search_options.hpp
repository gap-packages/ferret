#ifndef SEARCH_OPTIONS_EDCT
#define SEARCH_OPTIONS_EDCT

#include "library/library.hpp"

enum RBaseSearchHeuristic
{
    RBaseBranch_First,
    RBaseBranch_Largest,
    RBaseBranch_Smallest,
    RBaseBranch_Smallest2,
    RBaseBranch_Random,
    RBaseBranch_RandomSmallest
};

enum SearchHeuristic
{
    SearchBranch_RBase,
    SearchBranch_InvRBase,
    SearchBranch_Random,
    SearchBranch_Sorted,
    SearchBranch_Nosort,
};

RBaseSearchHeuristic getRBaseHeuristic(std::string sh)
{
    if(sh == "first")
        return RBaseBranch_First;
    else if(sh == "largest")
        return RBaseBranch_Largest;
    else if(sh == "smallest")
        return RBaseBranch_Smallest;
    else if(sh == "smallest2")
        return RBaseBranch_Smallest2;
    else if(sh == "random")
        return RBaseBranch_Random;
    else if(sh == "randomsmallest")
        return RBaseBranch_RandomSmallest;
    else
    {
        throw GAPException("Invalid rBase heuristic :" + sh);
    }
}


SearchHeuristic getSearchHeuristic(std::string sh)
{

    if(sh == "RBase")
        return SearchBranch_RBase;
    else if(sh == "InvRBase")
        return SearchBranch_InvRBase;
    else if(sh == "Random")
        return SearchBranch_Random;
    else if(sh == "Sorted")
        return SearchBranch_Sorted;
    else if(sh == "Nosort")
        return SearchBranch_Nosort;
    else
    {
        throw GAPException("Invalid search heuristic :" + sh);
    }
}



struct SearchOptions
{
    bool only_find_generators;
    bool find_canonical_perm;

    bool just_rbase;

    RBaseSearchHeuristic rbase_value_heuristic;
    RBaseSearchHeuristic rbase_cell_heuristic;

    SearchHeuristic search_value_heuristic;
    SearchHeuristic search_first_branch_value_heuristic;

    SearchOptions() :
    only_find_generators(false), find_canonical_perm(false), just_rbase(false),
    rbase_value_heuristic(RBaseBranch_First), rbase_cell_heuristic(RBaseBranch_First),
    search_value_heuristic(SearchBranch_Nosort), search_first_branch_value_heuristic(SearchBranch_Nosort)
    { }

};


#endif
