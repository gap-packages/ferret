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
        for(int i = 1; i <= trans.size(); ++i)
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
        for(int i = 1; i <= unpacked_stabChain_depth.size(); ++i)
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
    bool useOrbits;
    bool useBlocks;
    bool useOrbitals;

    StabChainConfig()
    : useOrbits(false), useBlocks(false), useOrbitals(false)
    { }

    StabChainConfig(bool uO, bool uB, bool uOtals)
    : useOrbits(uO), useBlocks(uB), useOrbitals(uOtals)
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

    RevertingStack<Permutation> last_permutation;
    Reverting<int> last_depth;

public:
    virtual std::string name() const
    {
        std::string s;
        if(config.useOrbits) s += "Orbit";
        if(config.useBlocks) s += "Block";
        if(config.useOrbitals) s += "Orbitals";
        return s + "StabChainInGroup";
     }


    StabChain_PermGroup(Obj _group, StabChainConfig _config, PartitionStack* ps, MemoryBacktracker* mb)
    : AbstractConstraint(ps), config(_config), scc(_group), 
      last_permutation(mb->makeRevertingStack<Permutation>()),
      last_depth(mb->makeReverting<int>())
    {
        original_partitions.resize(ps->domainSize()+1);

        if(config.useBlocks)
            original_blocks.resize(ps->domainSize() + 1);

        // We set up our 'reverting' at the start
        last_depth.set(0);
        last_permutation.push_back(Permutation());
    }
private:

    const vec1<int>& getRBasePartition(const vec1<int>& fix)
    {
        debug_out(3, "scpg", "Fixing: "<< fix);
        vec1<vec1<int> > oart = scc.orbits(fix, ps->domainSize());
        debug_out(3, "scpg", "Got orbit partition"<< oart);
        // This might not be necessary, but it doesn't hurt!
        for(int i = 1; i <= oart.size(); ++i)
            std::sort(oart[i].begin(), oart[i].end());
        std::sort(oart.begin(), oart.end());
        vec1<int> filter = partitionToList(oart, ps->domainSize(), MissingPoints_Fixed);
        debug_out(3, "scpg", "Filter partition: "<< filter);

        original_partitions[fix.size()+1] = MOVE(filter);

        return original_partitions[fix.size()+1];
    }

    vec1<std::map<int,int> >& getRBaseBlocks(const vec1<int>& fix)
    {
        vec1<vec1<vec1<int> > > blocks = scc.blocks(fix);
        vec1<std::map<int,int> > block_functions;
        for(int i = 1; i <= blocks.size(); ++i)
        {
            block_functions.push_back(partitionToMap(blocks[i]));
        }
        original_blocks[fix.size() + 1] = MOVE(block_functions);

        return original_blocks[fix.size() + 1];
    }

    const vec1<int>& getRBasePartition_cached(int s)
    {  return original_partitions[s+1]; }

    const vec1<std::map<int,int> >& getRBaseBlocks_cached(int s)
    {  return original_blocks[s+1]; }


public:

    std::vector<TriggerType> triggers()
    {
      std::vector<TriggerType> v;
      v.push_back(Trigger_Fix);
      v.push_back(Trigger_RBaseFinished);
      return v;
    }

    SplitState signal_start()
    {
        return signal_fix_buildingRBase(0);
    }

    virtual void signal_RBaseFinished(RBase* _rb)
    {
        rb = _rb;
        D_ASSERT(rb->value_ordering == ps->fixed_values());
        scc.initalize(rb->value_ordering);
    }

    virtual SplitState signal_fix_buildingRBase(int /*i*/)
    {
        debug_out(3, "scpg", "signal_fix_buildingRBase");
        const vec1<int>& part = getRBasePartition(ps->fixed_values());

        vec1<std::map<int,int> > blocks;

        if(config.useBlocks)
        {
            blocks = getRBaseBlocks(ps->fixed_values());
        }

        SplitState ss(true);
        if(config.useOrbits)
        {
            ss = filterPartitionStackByFunction(ps, ContainerToFunction(&part));
            if(ss.hasFailed())
                return ss;
        }

        if(config.useBlocks)
        {
            for(int i = 1; i <= blocks.size(); ++i)
            {
                ss = filterPartitionStackByUnorderedFunction(ps, SparseFunction<MissingPoints_Free>(&blocks[i]));
                if(ss.hasFailed())
                    return ss;
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

#ifndef NO_DEBUG
        for(int i = 1; i <= old_depth; ++i)
        { D_ASSERT(perm[new_fix_cells[i]] == (rb->value_ordering)[i]); }
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

#ifndef NO_DEBUG
        for(int i = 1; i <= new_depth; ++i)
        { D_ASSERT(perm[new_fix_cells[i]] == (rb->value_ordering)[i]); }
#endif

        const vec1<int>& part = getRBasePartition_cached(new_depth);

        SplitState ss(true);
        if(config.useOrbits)
        {
            ss = filterPartitionStackByFunction(ps, FunctionByPerm(ContainerToFunction(&part), perm));
            if(ss.hasFailed())
                return ss;
        }

        if(config.useBlocks)
        {
            const vec1<std::map<int,int> >& blocks = getRBaseBlocks_cached(new_depth);
            for(int i = 1; i <= blocks.size(); ++i)
            {
                ss = filterPartitionStackByUnorderedFunction(ps, FunctionByPerm(SparseFunction<MissingPoints_Free>(&blocks[i]), perm));
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
