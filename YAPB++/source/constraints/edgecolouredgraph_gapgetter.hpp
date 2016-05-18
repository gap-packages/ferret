#ifndef _EDGEGRAPH_GAPGETTER_ASD
#define _EDGEGRAPH_GAPGETTER_ASD

#include "library/graph.hpp"

namespace GAPdetail
{
template<>
struct GAP_getter<ColEdge>
{
    bool isa(Obj recval) const
    {
      GAP_getter<std::pair<int, int> > g;
      return g.isa(recval);
    }
    
    ColEdge operator()(Obj rec) const
    {
      GAP_getter<std::pair<int, int> > g;
      return ColEdge(g(rec));
    }
};

template<>
struct GAP_getter<UncolouredEdge>
{
    bool isa(Obj recval) const
    {
      GAP_getter<int> g;
      return g.isa(recval);
    }
    
    UncolouredEdge operator()(Obj rec) const
    {
      GAP_getter<int> g;
      return UncolouredEdge(g(rec), 0);
    }
};

}

#endif
