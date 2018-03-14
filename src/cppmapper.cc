

#include "gap_cpp_mapping.hpp"

#include "c_to_cpp.h"

#include "generated_headers/function_objs.h"
#include "generated_headers/RNamNames_list.h"

#include <ostream>
#include <iostream>

#include "problem.hpp"
#include "solution_store.hpp"
#include "search/search.hpp"
#include "search/search_options.hpp"

#include "constraints/liststab.hpp"
#include "constraints/setstab.hpp"
#include "constraints/setsetstab.hpp"
#include "constraints/settuplestab.hpp"
#include "constraints/overlappingsetset.hpp"
#include "constraints/edgecolouredgraph.hpp"
#include "constraints/edgecolouredgraph_gapgetter.hpp"
#include "constraints/perm_group.hpp"
#include "constraints/stabchain_perm_group.hpp"
#include "constraints/fixallpoints.hpp"

AbstractConstraint* buildConstraint(Obj con, PartitionStack* ps, MemoryBacktracker* mb, MemoryBacktracker* rbase_mb)
{
    char* conname = GAP_get<char*>(GAP_get_rec(con, RName_constraint));

    if(strcmp(conname, "SetStab") == 0)
    {
        return new SetStab(GAP_get<vec1<int> >(GAP_get_rec(con, RNamName("arg"))), ps);
    }
    if(strcmp(conname, "FixAllPoints") == 0)
    {
        return new FixAllPoints(ps);
    }
    else if(strcmp(conname, "SetSetStab") == 0)
    {
        return new SetSetStab(GAP_get<vec1<vec1<int> > >(GAP_get_rec(con, RNamName("arg"))), ps);
    }
    else if(strcmp(conname, "SetTupleStab") == 0)
    {
        return new SetTupleStab(GAP_get<vec1<vec1<int> > >(GAP_get_rec(con, RNamName("arg"))), ps);
    }
    else if(strcmp(conname, "OverlappingSetSetStab") == 0)
    {
        return new OverlapSetSetStab(GAP_get<vec1<vec1<int> > >(GAP_get_rec(con, RNamName("arg"))), ps);
    }
    else if(strcmp(conname, "ListStab") == 0)
    {
        return new ListStab(GAP_get<vec1<int> >(GAP_get_rec(con, RNamName("arg"))), ps);
    }
    else if(strcmp(conname, "DirectedGraph") == 0)
    {
        GraphConfig gc;
        if(GAP_has_rec(con, RNamName("start_path_length"))) {
            gc.start_path_length = GAP_get<int>(GAP_get_rec(con, RNamName("start_path_length")));
        }
        if(GAP_has_rec(con, RNamName("normal_path_length"))) {
            gc.normal_path_length = GAP_get<int>(GAP_get_rec(con, RNamName("normal_path_length")));
        }
        
        return new EdgeColouredGraph<UncolouredEdge, GraphDirected_yes>(
            GAP_get<vec1<vec1<UncolouredEdge> > >(GAP_get_rec(con, RNamName("arg"))), gc, ps);
    }
    else if(strcmp(conname, "EdgeColouredDirectedGraph") == 0)
    {
        GraphConfig gc;
        if(GAP_has_rec(con, RNamName("start_path_length"))) {
            gc.start_path_length = GAP_get<int>(GAP_get_rec(con, RNamName("start_path_length")));
        }
        if(GAP_has_rec(con, RNamName("normal_path_length"))) {
            gc.normal_path_length = GAP_get<int>(GAP_get_rec(con, RNamName("normal_path_length")));
        }
            
        return new EdgeColouredGraph<ColEdge, GraphDirected_yes>(
            GAP_get<vec1<vec1<ColEdge> > >(GAP_get_rec(con, RNamName("arg"))), gc, ps);
    }
    else if(strcmp(conname, "Generators_Inefficient") == 0)
    {
        return new PermGroup(GAP_get_rec(con, RNamName("arg")), ps);
    }
    else if(strcmp(conname, "Generators_StabChain") == 0)
    {
        StabChainConfig scc(GAP_get<std::string>(GAP_get_rec(con, RNamName("orbits"))),
                            GAP_get<std::string>(GAP_get_rec(con, RNamName("blocks"))),
                            GAP_get<std::string>(GAP_get_rec(con, RNamName("orbitals"))));
        return new StabChain_PermGroup(GAP_get_rec(con, RNamName("arg")), scc, ps, mb, rbase_mb);
    }
    else if(strcmp(conname, "NULL") == 0)
        return 0;

    else
        throw GAPException("Unknown constraint type: " + std::string(conname));
}

void readNestedConstraints_inner(Problem& p, Obj conlist, std::vector<AbstractConstraint*>& vec)
{
    vec1<Obj> cons = GAP_get<vec1<Obj> >(conlist);
    for(const auto& con : cons)
    {
        if(GAP_isa<vec1<Obj> >(con))
            readNestedConstraints_inner(p, con, vec);
        else
            vec.push_back(buildConstraint(con, &p.p_stack, &p.full_search_memory_backtracker, &p.rbase_generation_memory_backtracker));
            //p.addConstraint(buildConstraint(cons[i], &p.p_stack, &p.full_search_memory_backtracker));
    }
}

std::vector<AbstractConstraint*> readNestedConstraints(Problem& p, Obj conlist)
{
    std::vector<AbstractConstraint*> vec;
    readNestedConstraints_inner(p, conlist, vec);
    return vec;
}

SearchOptions fillSearchOptions(Obj options)
{
  SearchOptions so;

  so.only_find_generators = GAP_get<bool>(GAP_get_rec(options, RName_only_find_generators));
  //so.find_canonical_perm = GAP_get<bool>(GAP_get_rec(options, RName_canonical));

  Obj limit = GAP_get_rec(options, RName_nodeLimit);
  if(limit != False) {
      so.node_limit = GAP_get<long>(limit);
  }

  so.just_rbase = GAP_get<bool>(GAP_get_rec(options, RName_just_rbase));
  so.heuristic.rbase_value = getRBaseHeuristic(GAP_get<std::string>(GAP_get_rec(options, RName_rbaseValueHeuristic)));
  so.heuristic.rbase_cell = getRBaseHeuristic(GAP_get<std::string>(GAP_get_rec(options, RName_rbaseCellHeuristic)));
  so.heuristic.search_value = getSearchHeuristic(GAP_get<std::string>(GAP_get_rec(options, RName_searchValueHeuristic)));
  so.heuristic.search_first_branch_value = getSearchHeuristic(GAP_get<std::string>(GAP_get_rec(options, RName_searchFirstBranchValueHeuristic)));

  return so;
}

Obj getStatsRecord()
{
  Obj stats = NEW_PREC(0);

  AssPRec(stats, RNamName("nodes"), GAP_make(Stats::container().node_count));
  CHANGED_BAG(stats);
  AssPRec(stats, RNamName("fixedpoints"), GAP_make(Stats::container().rBase_fixed_points));
  CHANGED_BAG(stats);
  AssPRec(stats, RNamName("bad_leaves"), GAP_make(Stats::container().bad_leaves));
  CHANGED_BAG(stats);
  AssPRec(stats, RNamName("bad_internal_nodes"), GAP_make(Stats::container().bad_internal_nodes));
  CHANGED_BAG(stats);
  return stats;
}

Obj build_return_value(const SolutionStore& ss, bool get_stats)
{
  Obj rec = NEW_PREC(0);
  Obj sols = GAP_make(ss.sols());
  AssPRec(rec, RNamName("generators"),sols);
  CHANGED_BAG(rec);

  Obj rbasevalorder = GAP_make(Stats::container().rBase_value_ordering);
  AssPRec(rec, RNamName("rbase"), rbasevalorder);
  CHANGED_BAG(rec);

  Obj solsmap = GAP_make(ss.solsmap());
  AssPRec(rec, RNamName("generators_map"), solsmap);
  CHANGED_BAG(rec);

  if(get_stats)
  {
      Obj stats = getStatsRecord();
      AssPRec(rec, RNamName("stats"), stats);
      CHANGED_BAG(rec);
  }

#ifdef ENABLE_TIMING
    Obj timing_info = GAP_make(get_timing_info());
    AssPRec(rec, RNamName("timing"), timing_info);
    CHANGED_BAG(rec);
#endif

  return rec;
}

Obj solver(Obj conlist, Obj options)
{
    try{
        InfoLevel() = GAP_get<int>(GAP_callFunction(FunObj_getInfoFerret));
        DebugInfoLevel() = GAP_get<int>(GAP_callFunction(FunObj_getInfoFerretDebug));

        SearchOptions so = fillSearchOptions(options);

        bool get_stats = GAP_get<bool>(GAP_get_rec(options, RName_stats));
        int size = GAP_get<int>(GAP_get_rec(options, RName_size));

        Problem p(size);

        std::vector<AbstractConstraint*> cons = readNestedConstraints(p, conlist);

        SolutionStore ss = doSearch(&p, cons, so);

        Obj ret =  build_return_value(ss, get_stats);
        return ret;
    }
    catch(const GAPException& ge)
    {
        SyClearErrorNo();
        std::cerr << ge.what() << "\n";
        return Fail;
    }
}

Obj cosetSolver(Obj conlistCommon, Obj conlistL, Obj conlistR, Obj options)
{
    try{
        InfoLevel() = GAP_get<int>(GAP_callFunction(FunObj_getInfoFerret));
        DebugInfoLevel() = GAP_get<int>(GAP_callFunction(FunObj_getInfoFerretDebug));

        SearchOptions so = fillSearchOptions(options);

        bool get_stats = GAP_get<bool>(GAP_get_rec(options, RName_stats));
        int size = GAP_get<int>(GAP_get_rec(options, RName_size));

        Problem p(size);

        std::vector<AbstractConstraint*> consCommon = readNestedConstraints(p, conlistCommon);
        std::vector<AbstractConstraint*> consL = readNestedConstraints(p, conlistL);
        std::vector<AbstractConstraint*> consR = readNestedConstraints(p, conlistR);

        SolutionStore ss = doCosetSearch(&p, consCommon, consL, consR, so);

        Obj ret =  build_return_value(ss, get_stats);
        return ret;
    }
    catch(const GAPException& ge)
    {
        SyClearErrorNo();
        std::cerr << ge.what() << "\n";
        return Fail;
    }
}
