#ifndef _STABCHAIN_PERMGROUP_HPP_PLADIG
#define _STABCHAIN_PERMGROUP_HPP_PLADIG

#include <set>

#include "abstract_constraint.hpp"
#include "../partition_stack.hpp"
#include "../partition_refinement.hpp"
#include "../library/algorithms.hpp"
#include "../library/perm.hpp"
#include "gap_cpp_mapping.hpp"
#include "../../../src/StabChainStore.hpp"
#include "../rbase/rbase.hpp"
#include "../tunables.hpp"

typedef Graph<UncolouredEdge, GraphDirected_yes> OrbitalGraph;

// Stores a single level of a stabilizer chain
struct StabChainLevel
{
    GAPStabChainWrapper stabChain;
    int base_value;

    // Stores the perm that gets from 'i' to the base_value.
    vec1<optional<Permutation> > transversal;

    StabChainLevel(GAPStabChainWrapper _sc) :
    stabChain(_sc)
    {
        base_value = stabChain.getOrbitStart();
        transversal = stabChain.getTransversal();
    }

    bool check_transversal(const vec1<Permutation>& trans, int start, int end)
    {
        for(int i : range1(trans.size()))
        {
            start = trans[i][start];
        }
        return start == end;
    }

    Permutation complete_transversal(int i)
    {
        D_ASSERT(transversal[i]);
        vec1<Permutation> v;
        vec1<int> place;
        int current = i;
        while(current != base_value)
        {
            v.push_back(*transversal[current]);
            place.push_back(current);
            current = (*(transversal[current]))[current];
        }
        D_ASSERT(check_transversal(v, i, base_value));
        D_ASSERT((*transversal[place.back()])[place.back()] == base_value);

        while(v.size() > TUNABLE_STABCHAIN_MERGE_DIST)
        {
            vec1<Permutation> last_few(v.end() - TUNABLE_STABCHAIN_MERGE_DIST, v.end());

            int new_place = *(place.end() - TUNABLE_STABCHAIN_MERGE_DIST);
            D_ASSERT(check_transversal(last_few, new_place, base_value));
            transversal[new_place] = mergePermutations(last_few);

            D_ASSERT((*transversal[new_place])[new_place] == base_value);

            int new_end = v.size() - (TUNABLE_STABCHAIN_MERGE_DIST - 1);
            v[new_end] = *transversal[place[new_end]];
            v.resize(new_end);
            place.resize(new_end);


            D_ASSERT(check_transversal(v, i, base_value));
            D_ASSERT((*transversal[place.back()])[place.back()] == base_value);
        }

#if TUNABLE_STABCHAIN_RECORD_ALL_PERMS
       transversal[i] = mergePermutations(v);
       return *transversal[i];
#else
        return mergePermutations(v);
#endif
    }

    bool exists_perm_to(int i)
    {
        if(transversal.size() < i)
            return false;

        return transversal[i];
    }

    Permutation perm_mapping_from(int i)
    {
        if((*transversal[i])[i] == base_value)
            return *transversal[i];
        return complete_transversal(i);
    }
};

// This caches various parts of a stabilizer chain, calling out to GAP
// where required.
struct StabChainCache
{
    Obj group;
    Obj stabChain;
    // The individual levels of the stab chain unpacked
    vec1<StabChainLevel> levels;

    // As we reduce the stab chain, this tells us, for each depth,
    // where this depth is compressed to.
    vec1<int> unpacked_stabChain_depth;

    // Just an error check, for when we move from initial phase into final phase
    bool fixed_base;

    StabChainCache(Obj _group) : group(_group), fixed_base(false)
    {
        GAP_addRef(group);
        Obj sc  = GAP_callFunction(FunObj_CopyStabChain,GAP_callFunction(FunObj_StabChainMutable, group));
        GAP_addRef(sc);
        stabChain = sc;
    }

    StabChainLevel& getUnpackedLevel(int i)
    { return levels[unpacked_stabChain_depth[i]]; }

    bool hasUnpackedLevel(int i)
    { return unpacked_stabChain_depth[i] != 0; }


    GAPStabChainWrapper getscc(const vec1<int>& v)
    {
        D_ASSERT(!fixed_base);
        GAP_callFunction(FunObj_ChangeStabChain, stabChain, GAP_make(v));
        GAPStabChainWrapper scc(stabChain);
        if(v.empty())
            return scc;
        // Now we have to walk down the stabilizer chain, until we find the position where v ends
        int pos = 1;
        while(true)
        {
            int orbit = scc.getOrbitStart();
            while(pos <= v.size() && v[pos] != orbit)
                pos++;
            if(pos > v.size())
            {
                return scc;
            }
            if(!scc.hasNextLevel())
                return scc;
            scc = scc.getNextLevel();
            if(!scc.hasOrbit())
            {
                // Reached bottom, so exit early. getOrbitsPartition still handles this case.
                return scc;
            }
        }
    }

    vec1<vec1<int> > orbits(const vec1<int>& v, int domain_size)
    { return getscc(v).getOrbitsPartition(domain_size); }

    vec1<vec1<vec1<int> > > blocks(const vec1<int>& v)
    { return getscc(v).getBlockList(); }

    vec1<OrbitalGraph> orbitals(const vec1<int>& v, int domain_size)
    { return getscc(v).getOrbitalList(domain_size); }

    void initalize(const vec1<int>& order)
    {
        D_ASSERT(!fixed_base);
        fixed_base = true;
        unpacked_stabChain_depth.resize(order.size());
        GAP_callFunction(FunObj_ChangeStabChain, stabChain, GAP_make(order));

        debug_out(1, "SCC", "Setting up cache");
        debug_out(3, "SCC", "Order " << order);
        int order_pos = 1;

        GAPStabChainWrapper stabChainCpy(stabChain);
        do
        {
            StabChainLevel scl(stabChainCpy);

            while(order[order_pos] != scl.base_value)
            {
                debug_out(3, "SCC", "Skipping depth " << order_pos);
                order_pos++;
            }

            debug_out(3, "SCC", "Setting depth "<<order_pos<<" base point "<<scl.base_value);
            levels.push_back(scl);
            unpacked_stabChain_depth[order_pos] = levels.size();

            stabChainCpy = stabChainCpy.getNextLevel();
        }
        while(stabChainCpy.hasNextLevel());

#ifndef NO_DEBUG
        for(int i : range1(unpacked_stabChain_depth.size()))
        {
            if(unpacked_stabChain_depth[i] != 0)
            {
                D_ASSERT(levels[unpacked_stabChain_depth[i]].base_value == order[i]);
            }
        }
#endif
    }
};

struct StabChainConfig
{
    enum sc_config_option {
        never,
        always,
        root,
        firstnontrivial,
        firstnontrivialwithroot
    };

    // For these cases, we need to store the depth at which we found the first non-trivial
    // orbit / block / orbital
    static bool doStoreNontrivial(StabChainConfig::sc_config_option o)
    { return o == firstnontrivial || o == firstnontrivialwithroot; }

    // In these cases we do an extra check at the root, where we consider the initial group
    // without any extra propagation
    static bool doRootCheck(StabChainConfig::sc_config_option o)
    { return o == root || o == firstnontrivialwithroot; }

    static bool doConsiderEveryNode(StabChainConfig::sc_config_option o)
    { return o == always || o == firstnontrivial || o == firstnontrivialwithroot; }


    sc_config_option optionFromString(std::string s) {
        if(s == "never") return never;
        if(s == "always") return always;
        if(s == "root") return root;
        if(s == "firstnontrivial") return firstnontrivial;
        if(s == "firstnontrivialwithroot") return firstnontrivialwithroot;
        
        throw GAPException("'" + s + "' is not a valid configuration option for ConInGroup."
                           "Valid options are never, always, root, firstnontrivial, firstnontrivialwithroot");
    }

    sc_config_option useOrbits;
    sc_config_option useBlocks;
    sc_config_option useOrbitals;

    StabChainConfig()
    : useOrbits(never), useBlocks(never), useOrbitals(never)
    { }


    StabChainConfig(const std::string& uO, const std::string& uB, const std::string& uOtals)
    : useOrbits(optionFromString(uO)),
      useBlocks(optionFromString(uB)),
      useOrbitals(optionFromString(uOtals))
    { }
};



class StabChain_PermGroup : public AbstractConstraint
{
    StabChainConfig config;
    StabChainCache scc;
    //vec1<vec1<vec1<int> > >
    RBase* rb;

    vec1<vec1<int> > original_partitions;

    vec1<vec1< std::map<int,int> > > original_blocks;

    vec1<vec1<OrbitalGraph> > original_orbitals;

    RevertingStack<Permutation> last_permutation;
    Reverting<int> last_depth;

    // These variables are used to track where we find the 
    // first non-trivial cases during rBase construction
    Reverting<int> tracking_first_found_orbits;
    Reverting<int> tracking_first_found_blocks;
    Reverting<int> tracking_first_found_orbitals;

    // These are filled in once the rbase is constructed
    int first_found_orbits;
    int first_found_blocks;
    int first_found_orbitals;

public:
    virtual std::string name() const
    {
        std::string s;
        if(config.useOrbits) s += "Orbits:";
        if(config.useBlocks) s += "Blocks:";
        if(config.useOrbitals) s += "Orbitals:";
        return s + "StabChainInGroup";
     }


    StabChain_PermGroup(Obj _group, StabChainConfig _config, PartitionStack* ps, MemoryBacktracker* mb)
    : AbstractConstraint(ps), config(_config), scc(_group), 
      last_permutation(mb->makeRevertingStack<Permutation>()),
      last_depth(mb->makeReverting<int>())
    {
        first_found_blocks = first_found_blocks = first_found_orbitals = -2;

        if(StabChainConfig::doStoreNontrivial(config.useOrbits))
        { tracking_first_found_orbits = Reverting<int>(mb->makeReverting<int>(-2)); }

        if(StabChainConfig::doStoreNontrivial(config.useBlocks))
        { tracking_first_found_blocks = Reverting<int>(mb->makeReverting<int>(-2)); }

        if(StabChainConfig::doStoreNontrivial(config.useOrbitals))
        { tracking_first_found_orbitals = Reverting<int>(mb->makeReverting<int>(-2)); }

        // We set up our 'reverting' at the start
        last_depth.set(-1);
        last_permutation.push_back(Permutation());

        debug_out(3, "scpg", "Setup");
    }
private:

    const vec1<int>* getRBaseOrbitPartition(const vec1<int>& fix)
    {
        debug_out(3, "scpg", "Fixing: "<< fix);
        vec1<vec1<int> > oart = scc.orbits(fix, ps->domainSize());
        debug_out(3, "scpg", "Got orbit partition" << oart);
        // This might not be necessary, but it doesn't hurt!
        for(int i : range1(oart.size()))
            std::sort(oart[i].begin(), oart[i].end());
        std::sort(oart.begin(), oart.end());

        vec1<int> filter;
        if(oart.size() > 1)
            filter = partitionToList(oart, ps->domainSize(), MissingPoints_Fixed);

        debug_out(3, "scpg", "Filter partition: "<< filter);

        if(original_partitions.size() < fix.size() + 1)
            original_partitions.resize(fix.size() + 1);

        original_partitions[fix.size()+1] = std::move(filter);

        return &(original_partitions[fix.size()+1]);
    }

    vec1<std::map<int,int> >* getRBaseBlocks(const vec1<int>& fix)
    {
        vec1<vec1<vec1<int> > > blocks = scc.blocks(fix);
        vec1<std::map<int,int> > block_functions;
        for(int i : range1(blocks.size()))
        {
            block_functions.push_back(partitionToMap(blocks[i]));
        }

        if(original_blocks.size() < fix.size() + 1)
            original_blocks.resize(fix.size() + 1);

        original_blocks[fix.size() + 1] = std::move(block_functions);

        return &(original_blocks[fix.size() + 1]);
    }

    vec1<OrbitalGraph>* getRBaseOrbitals(const vec1<int>& fix)
    {
        vec1<OrbitalGraph> orbitals = scc.orbitals(fix, ps->domainSize());

        if(original_orbitals.size() < fix.size() + 1)
            original_orbitals.resize(fix.size() + 1);
    
        original_orbitals[fix.size() + 1] = std::move(orbitals);
        return &(original_orbitals[fix.size() + 1]);
    }

    const vec1<int>* getRBaseOrbitPartition_cached(int s)
    {  return &(original_partitions[s+1]); }

    const vec1<std::map<int,int> >* getRBaseBlocks_cached(int s)
    {  return &(original_blocks[s+1]); }

    const vec1<OrbitalGraph>* getRBaseOrbitals_cached(int s)
    { return &(original_orbitals[s+1]); }

public:

    std::vector<TriggerType> triggers()
    {
      std::vector<TriggerType> v;
      v.push_back(Trigger_Fix);
      v.push_back(Trigger_RBaseFinished);
      if(config.useBlocks != StabChainConfig::never || config.useOrbitals != StabChainConfig::never)
        v.push_back(Trigger_Change);
      return v;
    }

    // We only want to do 'changed' propagation for blocks and orbitals
    // and then, only when we don't expect it to be handled by a 'fix' in future.
    SplitState signal_changed(const vec1<int>& cells)
    {
        D_ASSERT(last_depth.get() <= ps->fixed_values().size());
        if(last_depth.get() == ps->fixed_values().size())
        {

        }
        SplitState ss(true);
        return ss;
    }

    SplitState signal_start()
    {
        SplitState root = fix_buildingRBase(vec1<int>(),
                                            StabChainConfig::doRootCheck(config.useOrbits),
                                            StabChainConfig::doRootCheck(config.useBlocks),
                                            StabChainConfig::doRootCheck(config.useOrbitals));
        if(root.hasFailed()) return root;

        return root;
    }

    virtual void signal_RBaseFinished(RBase* _rb)
    {
        rb = _rb;
        D_ASSERT(rb->value_ordering == ps->fixed_values());
        scc.initalize(rb->value_ordering);

        if(StabChainConfig::doStoreNontrivial(config.useOrbits))
            first_found_orbits = tracking_first_found_orbits.get();

        if(StabChainConfig::doStoreNontrivial(config.useBlocks))
        first_found_blocks = tracking_first_found_blocks.get();

        if(StabChainConfig::doStoreNontrivial(config.useOrbitals))
        first_found_orbitals = tracking_first_found_orbitals.get();
    }

    virtual SplitState signal_fix_buildingRBase(int /*i*/)
    {
        debug_out(3, "scpg", "signal_fix_buildingRBase");
        return fix_buildingRBase(ps->fixed_values(),
                                 StabChainConfig::doConsiderEveryNode(config.useOrbits),
                                 StabChainConfig::doConsiderEveryNode(config.useBlocks),
                                 StabChainConfig::doConsiderEveryNode(config.useOrbitals));
    }

    // This horrible function just wraps up something we want to do several times.
    // basically, if we have set a nontrivial depth, get the cached value at that
    // depth, else calculate a new value, and see if it is non-trivial.
    template<typename StorePoint, typename Func, typename CacheFunc>
    void doCacheCheck(StabChainConfig::sc_config_option configchoice,
                      Reverting<int>& nontrivialdepth,
                      StorePoint& ptr,
                      Func get, CacheFunc cached_get,
                      const vec1<int>& fixed_values, const char* name)
    {
        if(StabChainConfig::doStoreNontrivial(configchoice) )
            {
                //std::cerr << ":" << name << "\n";
                if(nontrivialdepth.get() >= 0)
                {
                    //std::cerr << "Using cached depth: " << nontrivialdepth.get() << "\n"; 
                    ptr = cached_get(nontrivialdepth.get() ); 
                }
                else
                {
                    ptr = get(fixed_values);
                    //std::cerr << "Found: " << *ptr << "\n";
                    if(ptr->size() > 0)
                    {
                        //std::cerr << "Setting cache depth: " << fixed_values.size() << "\n";
                        nontrivialdepth.set(fixed_values.size());
                    }
                }
            }
            else
            { ptr = get(fixed_values); }
    }

    SplitState fix_buildingRBase(const vec1<int>& fixed_values, bool useOrbits, bool useBlocks, bool useOrbitals)
    {
        debug_out(3, "scpg", "last depth " << last_depth.get() << " new depth " << fixed_values.size());
        D_ASSERT(fixed_values.size() > last_depth.get());
        last_depth.set(fixed_values.size());

        const vec1<int>* part = 0;
        const vec1<std::map<int,int> >* blocks = 0;
        const vec1<OrbitalGraph>* orbitals = 0;

        if(useOrbits)
        {
            doCacheCheck(config.useOrbits, tracking_first_found_orbits, part,
                         [this](const vec1<int>& v){ return this->getRBaseOrbitPartition(v); },
                         [this](int i){ return this->getRBaseOrbitPartition_cached(i); },
                         fixed_values, "orbits");
        }

        if(useBlocks)
        { 
             doCacheCheck(config.useBlocks, tracking_first_found_blocks, blocks,
                         [this](const vec1<int>& v){ return this->getRBaseBlocks(v); },
                         [this](int i){ return this->getRBaseBlocks_cached(i); },
                         fixed_values, "blocks");
        }

        if(useOrbitals)
        { 
            doCacheCheck(config.useOrbitals, tracking_first_found_orbitals, orbitals,
                         [this](const vec1<int>& v){ return this->getRBaseOrbitals(v); },
                         [this](int i){ return this->getRBaseOrbitals_cached(i); },
                         fixed_values, "orbitals");
        }

        SplitState ss(true);
        if(part)
        {
            debug_out(3, "scpg", "fix_rBase:orbits");
            if(!part->empty())
                ss = filterPartitionStackByFunction(ps, SquareBrackToFunction(part));
            if(ss.hasFailed())
                return ss;
        }

        if(blocks)
        {
            for(int i : range1(blocks->size()))
            {
                debug_out(3, "scpg", "fix_rBase:blocks" << (*blocks)[i] );
                ss = filterPartitionStackByUnorderedFunction(ps, SparseFunction<MissingPoints_Free>(&(*blocks)[i]));
                if(ss.hasFailed())
                    return ss;
            }
        }

        if(orbitals)
        {
            for(const auto& graph : (*orbitals))
            {
                debug_out(3, "scpg", "fix_rBase:orbitals"  << graph);
                GraphRefiner gr(ps->domainSize());
                vec1<int> cells;
                for(int i : range1(ps->cellCount()))
                    cells.push_back(i);
                ss = gr.filterGraph(ps, graph, cells, 1);
                if(ss.hasFailed())
                {
                    debug_out(3, "scpg", "Orbital failed");
                    return ss;
                }
            }
        }

        return ss;
    }

    virtual SplitState signal_fix(int /*fix_pos*/)
    {
        debug_out(3, "scpg", "signal_fix");
        const vec1<int>& new_fix_cells = ps->fixed_values();

        Permutation perm(last_permutation.back());
        int old_depth = last_depth.get();

        // We have to do this, as new_fix_cells changes as the function progresses
        int new_depth = new_fix_cells.size();

        D_ASSERT(new_depth > old_depth);

#ifndef NO_DEBUG
        for(int i : range1(old_depth))
        { 
            (void)i;
            D_ASSERT(perm[new_fix_cells[i]] == (rb->value_ordering)[i]);
        }
#endif

        PermutationStack perm_stack(perm);

        for(int i = old_depth + 1; i <= new_depth; ++i)
        {
            if(scc.hasUnpackedLevel(i))
            {
                StabChainLevel& scl = scc.getUnpackedLevel(i);
                D_ASSERT(scl.base_value == (rb->value_ordering)[i]);
                debug_out(3, "scpg", "Trying to map "<<new_fix_cells[i]<<" to "<<(rb->value_ordering)[i]);
                int image = perm_stack[new_fix_cells[i]];
                debug_out(3, "scpg", "Pre-image of "<<new_fix_cells[i]<<" is "<<image);
                if(!scl.exists_perm_to(image))
                {
                    debug_out(3, "scpg", "No perm exists to map to image");
                    return SplitState(false);
                }
                const optional<Permutation>& p = scl.perm_mapping_from(image);
                if(!p)
                {
                    debug_out(3, "scpg", "No perm mapping from image?");
                    return SplitState(false);
                }
                D_ASSERT((*p)[image] == (rb->value_ordering)[i]);
                debug_out(3, "scpg", "Old stack: " << perm_stack.getPerm());
                perm_stack.addPerm((*p));
                debug_out(3, "scpg", "New perm: " << *p << " added to stack gives " << perm_stack.getPerm());
                D_ASSERT(perm_stack[new_fix_cells[i]] == (rb->value_ordering)[i]);
            }
            else
            {
                if(perm_stack[new_fix_cells[i]] != (rb->value_ordering)[i])
                    return SplitState(false);
            }
        }

        perm = perm_stack.getPerm();
        last_permutation.push_back(perm);
        last_depth.set(new_depth);

// Check our permutation maps known points in the right way.
#ifndef NO_DEBUG
        for(int i : range1(new_depth))
        {
            (void)i;
            D_ASSERT(perm[new_fix_cells[i]] == (rb->value_ordering)[i]);
        }
#endif


        SplitState ss(true);
        if(StabChainConfig::doConsiderEveryNode(config.useOrbits))
        {
            int depth = new_depth;
            bool skip = false;
            if(StabChainConfig::doStoreNontrivial(config.useOrbits)) {
                int orbit_depth = first_found_orbits;
                debug_out(3, "scpg", "orbit check" << orbit_depth << ":" << depth);
                if(orbit_depth > depth || orbit_depth < 0)
                    skip = true;
                if(orbit_depth < depth)
                    depth = orbit_depth;
            }
            if(!skip) {
                const vec1<int>* part = getRBaseOrbitPartition_cached(depth);
                debug_out(3, "scpg", "fix:orbits" << part << " by " << perm);
                if(!part->empty())
                    ss = filterPartitionStackByFunction(ps, FunctionByPerm(SquareBrackToFunction(part), perm));
                if(ss.hasFailed())
                    return ss;
            }
        }

        if(StabChainConfig::doConsiderEveryNode(config.useBlocks))
        {
            ss = filterBlocks(new_depth, [&perm](const std::map<int,int>* blockptr)
                              { return FunctionByPerm(SparseFunction<MissingPoints_Free>(blockptr), perm); });
            if(ss.hasFailed())
                return ss;
        }

        if(StabChainConfig::doConsiderEveryNode(config.useOrbitals))
        {
          ss = filterOrbitals(new_depth, [&perm](const OrbitalGraph* graphptr){ return PermutedGraph<OrbitalGraph>(graphptr, perm); });
          if(ss.hasFailed())
            return ss;
        }
        return ss;
    }

    // Perform graph filtering, assuming 'new_depth' fixed points.
    // Apply function 'abm' to blocks, which will either be the identity function,
    // (when building rbase), or the permutation to apply to the graph (otherwise)
    template<typename ApplyBlockMapping>
    SplitState filterBlocks(int new_depth, const ApplyBlockMapping& abm)
    {
        SplitState ss(true);

        int depth = new_depth;
        bool skip = false;
        if(StabChainConfig::doStoreNontrivial(config.useBlocks)) {
            int orbit_depth = first_found_blocks;
            debug_out(3, "scpg", "block check" << orbit_depth << ":" << depth);
            if(orbit_depth > depth || orbit_depth < 0)
                skip = true;
            if(orbit_depth < depth)
                depth = orbit_depth;
        }

        if(!skip) {
            const vec1<std::map<int,int> >* blocks = getRBaseBlocks_cached(depth);
            for(const auto& block : *blocks)
            {
                debug_out(3, "scpg", "fix:blocks" << blocks[i] << " by " << perm);
                ss = filterPartitionStackByUnorderedFunction(ps, abm(&block));
                if(ss.hasFailed())
                    return ss;
            }
        }
        return ss;
    }
    
    // Perform graph filtering, assuming 'new_depth' fixed points.
    // Apply function 'agm' to graphs, which will either be the identity function,
    // (when building rbase), or the permutation to apply to the graph (otherwise)
    template<typename ApplyGraphMapping>
    SplitState filterOrbitals(int new_depth, const ApplyGraphMapping& agm)
    {
        SplitState ss(true);

        int depth = new_depth;
        bool skip = false;
        if(StabChainConfig::doStoreNontrivial(config.useOrbitals)) {
            int orbit_depth = first_found_orbitals;
            debug_out(3, "scpg", "orbital check" << orbit_depth << ":" << depth);
            if(orbit_depth > depth || orbit_depth < 0)
                skip = true;
            if(orbit_depth < depth)
                depth = orbit_depth;
        }

        if(!skip) {
            const vec1<OrbitalGraph>* orbitals = getRBaseOrbitals_cached(depth);
            //std::cerr << new_depth << ":" << depth << ":" << *orbitals << std::endl;

            for(const auto& graph : (*orbitals))
            {
                debug_out(3, "scpg", "fix:orbitals" << graph << " by " << perm);
                GraphRefiner gr(ps->domainSize());
                vec1<int> cells;
                for(int i : range1(ps->cellCount()))
                    cells.push_back(i);
                ss = gr.filterGraph(ps, agm(&graph), range1(ps->cellCount()), 1);
                if(ss.hasFailed())
                    return ss;
            }
        }
        
        return ss;
    }

    virtual bool verifySolution(const Permutation& p)
    { return GAP_get<bool>(GAP_callFunction(FunObj_inGroup, GAP_make(p), scc.group)); }
};


#endif
