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
    RBaseBranch_RandomSmallest,
    RBaseBranch_ConstraintAdvise,
};

enum SearchHeuristic
{
    SearchBranch_RBase,
    SearchBranch_InvRBase,
    SearchBranch_Random,
    SearchBranch_Sorted,
    SearchBranch_Nosort,
};

inline RBaseSearchHeuristic getRBaseHeuristic(std::string sh)
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


inline SearchHeuristic getSearchHeuristic(std::string sh)
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


struct Heuristic
{
  RBaseSearchHeuristic rbase_value;
  RBaseSearchHeuristic rbase_cell;

  SearchHeuristic search_value;
  SearchHeuristic search_first_branch_value;

  Heuristic() :
  rbase_value(RBaseBranch_Smallest), rbase_cell(RBaseBranch_Smallest),
  search_value(SearchBranch_RBase), search_first_branch_value(SearchBranch_RBase)
  { }
  
  static Heuristic randomHeuristic()
  {
      Heuristic h;
      h.rbase_value = RBaseBranch_Random;
      h.rbase_cell = RBaseBranch_Random;
      h.search_value = SearchBranch_Random;
      h.search_first_branch_value = SearchBranch_Random;
      return h;
  }

  static Heuristic scfHeuristic()
  {
      Heuristic h;
      h.rbase_value = RBaseBranch_Smallest;
      h.rbase_cell = RBaseBranch_Smallest;
      h.search_value = SearchBranch_Nosort;
      h.search_first_branch_value = SearchBranch_RBase;
      return h;
  }

  static Heuristic adviseHeuristic()
  {
      Heuristic h;
      h.rbase_value = RBaseBranch_Smallest;
      h.rbase_cell = RBaseBranch_ConstraintAdvise;
      h.search_value = SearchBranch_Nosort;
      h.search_first_branch_value = SearchBranch_RBase;
      return h;
  }

  static Heuristic orderHeuristic()
  {
      Heuristic h;
      h.rbase_value = RBaseBranch_First;
      h.rbase_cell = RBaseBranch_First;
      h.search_value = SearchBranch_Sorted;
      h.search_first_branch_value = SearchBranch_Sorted;
      return h;
  }
};


struct SearchOptions
{
    bool only_find_generators;
    bool find_canonical_perm;

    bool just_rbase;

    Heuristic heuristic;

    long long node_limit;

    SearchOptions() :
    only_find_generators(true), find_canonical_perm(false), just_rbase(false), node_limit(-1)
    { }

};


#endif
