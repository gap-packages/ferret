#include "gap_cpp_mapping.hpp"
#include "generated_headers/RNamNames_list.h"

#include "library/perm.hpp"
#include "library/optional.hpp"

// This class just provides a thin wrapper around a
// GAP stabilizer chain, to avoid having to remember the
// GAP notation. This is (obviously) not particularly efficient
// and does not do good caching. It is expected that anyone who
// uses this will keep an efficient cache themselves.

class GAPStabChainWrapper
{
    Obj sc;
public:
    GAPStabChainWrapper(Obj _sc) : sc(_sc)
    { }

    Obj getGAPStabChain() const
    { return sc; }

    bool hasNextLevel() const
    { return ISB_REC(sc, RName_stabilizer); }

    GAPStabChainWrapper getNextLevel() const
    { return GAPStabChainWrapper(ELM_REC(sc, RName_stabilizer)); }

    bool hasOrbit() const
    { return ISB_REC(sc, RName_orbit); }

    vec1<optional<Permutation> > getTransversal() const
    {
        vec1<optional<int> > translabels(getTranslabels());
        vec1<Permutation> labels(getLabels());

        vec1<optional<Permutation> > p(translabels.size(), optional<Permutation>());
        for(int i : range1(translabels.size()))
        {
            if(translabels[i])
            {
                p[i] = labels[*(translabels[i])];
            }
        }
        return p;
    }

    vec1<optional<int> > getTranslabels() const
    { return GAP_get<vec1<optional<int> > >(ELM_REC(sc, RName_translabels)); }

    vec1<Permutation> getLabels() const
    { return GAP_get<vec1<Permutation> >(ELM_REC(sc, RName_labels)); }

    vec1<int> getOrbit() const
    { //printf("GetOrbit."); ELM_REC(sc, RName_orbit); printf("!\n"); 
    return GAP_get<vec1<int> >(ELM_REC(sc, RName_orbit)); }

    vec1<vec1<int> > getOrbitsPartition(int dom) const
    { 
        if(!hasOrbit())
        {
            vec1<vec1<int> > v(dom);
            for(int i : range1(v.size()))
                v[i].push_back(i);
            return v;
        }
        else
            return GAP_get<vec1<vec1<int> > >(GAP_callFunction(FunObj_getOrbitPart, sc, GAP_make(dom))); 
    }

    vec1<vec1<vec1<int> > > getBlockList() const
    { 
        if(!hasOrbit())
        {
            return vec1<vec1<vec1<int> > >();
        }
        else
            return GAP_get<vec1<vec1<vec1<int> > > >(GAP_callFunction(FunObj_getBlockList, sc)); 
    }

    vec1<Graph<UncolouredEdge, GraphDirected_yes> > getOrbitalList(int domain) const
    {
        if(!hasOrbit())
        {
            return vec1<Graph<UncolouredEdge, GraphDirected_yes> >();
        }
        else
        {
            vec1<vec1<vec1<UncolouredEdge> > > graphvecs;
            graphvecs = GAP_get<vec1<vec1<vec1<UncolouredEdge> > > >(GAP_callFunction(FunObj_getOrbitalList, sc, GAP_make(domain)));
            vec1<Graph<UncolouredEdge, GraphDirected_yes> > graphs;
            for(const auto& edgesets : graphvecs)
            {
                graphs.push_back(Graph<UncolouredEdge, GraphDirected_yes>(edgesets, domain));
            }
            return graphs;
        }
    }

    int getOrbitStart() const
    { 
       // printf("getOrbitStart.");
        Obj o = ELM_REC(sc, RName_orbit);
    //    ELM_REC(sc, RName_orbit); printf("!\n"); 
        return GAP_get<int>(ELM_LIST(o, 1));
    }

    int transversalSize() const
    { return LEN_LIST(ELM_REC(sc, RName_transversal)); }

};
