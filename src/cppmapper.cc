

#include "gap_helper.h"

#include "c_to_cpp.h"

#include "generated_headers/function_objs.h"
#include "generated_headers/RNamNames_list.h"

#include <ostream>
#include <iostream>

#include "problem.hpp"
#include "solution_store.hpp"
#include "search.hpp"
#include "search_options.hpp"

#include "constraints/liststab.hpp"
#include "constraints/setstab.hpp"
#include "constraints/setsetstab.hpp"
#include "constraints/overlappingsetset.hpp"
#include "constraints/slowgraph.hpp"
#include "constraints/perm_group.hpp"
#include "constraints/stabchain_perm_group.hpp"
#include "constraints/fixallpoints.hpp"

AbstractConstraint* buildConstraint(Obj con, PartitionStack* ps, MemoryBacktracker* mb)
{
    char* conname = GAP_get<char*>(GAP_get_rec(con, RName_constraint));

    if(strcmp(conname, "SetStab") == 0)
    {
        return new SetStab(GAP_get<vec1<int> >(GAP_get_rec(con, RName_arg)), ps);
    }
    if(strcmp(conname, "FixAllPoints") == 0)
    {
        return new FixAllPoints(ps);
    }
    else if(strcmp(conname, "SetSetStab") == 0)
    {
        return new SetSetStab(GAP_get<vec1<vec1<int> > >(GAP_get_rec(con, RName_arg)), ps);
    }
    else if(strcmp(conname, "OverlappingSetSetStab") == 0)
    {
        return new OverlapSetSetStab(GAP_get<vec1<vec1<int> > >(GAP_get_rec(con, RName_arg)), ps);
    }
    else if(strcmp(conname, "ListStab") == 0)
    {
        return new ListStab(GAP_get<vec1<int> >(GAP_get_rec(con, RName_arg)), ps);
    }
    else if(strcmp(conname, "DirectedGraph") == 0)
    {
        return new SlowGraph<GraphDirected_yes>(
            GAP_get<vec1<vec1<int> > >(GAP_get_rec(con, RName_arg)), ps);
    }
    else if(strcmp(conname, "Generators_Inefficient") == 0)
    {
        return new PermGroup(GAP_get_rec(con, RName_arg), ps);
    }
    else if(strcmp(conname, "Generators_OrbStabChain") == 0)
    {
        return new StabChain_PermGroup<true, false>(GAP_get_rec(con, RName_arg), ps, mb);
    }
    else if(strcmp(conname, "Generators_BlockOrbStabChain") == 0)
    {
        return new StabChain_PermGroup<true, true>(GAP_get_rec(con, RName_arg), ps, mb);
    }
    else if(strcmp(conname, "Generators_StabChain") == 0)
    {
        return new StabChain_PermGroup<false, false>(GAP_get_rec(con, RName_arg), ps, mb);
    }
    else if(strcmp(conname, "Generators_BlockStabChain") == 0)
    {
        return new StabChain_PermGroup<false, true>(GAP_get_rec(con, RName_arg), ps, mb);
    }

    else
        throw GAPException("Unknown constraint type: " + std::string(conname));
}

void readNestedConstraints(Problem& p, Obj conlist)
{
    vec1<Obj> cons = GAP_get<vec1<Obj> >(conlist);
    for(int i = 1; i <= cons.size(); ++i)
    {
        if(GAP_isa<vec1<Obj> >(cons[i]))
            readNestedConstraints(p, cons[i]);
        else
            p.addConstraint(buildConstraint(cons[i], &p.p_stack, &p.memory_backtracker));
    }
}

Obj solver(Obj conlist, Obj options)
{
    try{
        SearchOptions so;

        so.only_find_generators = GAP_get<bool>(GAP_get_rec(options, RName_only_find_generators));
        //so.find_canonical_perm = GAP_get<bool>(GAP_get_rec(options, RName_canonical));
        so.just_rbase = GAP_get<bool>(GAP_get_rec(options, RName_just_rbase));
        so.heuristic.rbase_value = getRBaseHeuristic(GAP_get<std::string>(GAP_get_rec(options, RName_rbaseValueHeuristic)));
        so.heuristic.rbase_cell = getRBaseHeuristic(GAP_get<std::string>(GAP_get_rec(options, RName_rbaseCellHeuristic)));
        so.heuristic.search_value = getSearchHeuristic(GAP_get<std::string>(GAP_get_rec(options, RName_searchValueHeuristic)));
        so.heuristic.search_first_branch_value = getSearchHeuristic(GAP_get<std::string>(GAP_get_rec(options, RName_searchFirstBranchValueHeuristic)));



        bool get_stats = GAP_get<bool>(GAP_get_rec(options, RName_stats));

        int size = GAP_get<int>(GAP_get_rec(options, RName_size));
        Problem p(size);

        readNestedConstraints(p, conlist);

        SolutionStore ss = doSearch(&p, so);

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
            Obj stats = NEW_PREC(0);

            AssPRec(stats, RNamName("nodes"), GAP_make(Stats::container().node_count));
            CHANGED_BAG(stats);
            AssPRec(stats, RNamName("fixedpoints"), GAP_make(Stats::container().rBase_fixed_points));
            CHANGED_BAG(stats);
            AssPRec(stats, RNamName("bad_leaves"), GAP_make(Stats::container().bad_leaves));
            CHANGED_BAG(stats);
            AssPRec(stats, RNamName("bad_internal_nodes"), GAP_make(Stats::container().bad_internal_nodes));
            CHANGED_BAG(stats);
            AssPRec(rec, RNamName("stats"), stats);
            CHANGED_BAG(rec);
        }
        return rec;
    }
    catch(const GAPException& ge)
    {
        SyClearErrorNo();
        std::cerr << ge.what() << "\n";
        return Fail;
    }
}
