#ifndef _EDGEGRAPH_GAPGETTER_ASD
#define _EDGEGRAPH_GAPGETTER_ASD

#include "edgecolouredgraph.hpp"

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
}

#endif
