#define YAPB_NO_GAP
#include "simple_graph.h"

#include "problem.hpp"
#include "constraints/slowgraph.hpp"
#include "constraints/setstab.hpp"
#include "search.hpp"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>

vec1<Permutation> SolveGraph(const Graph &g, SearchOptions so, GraphDirected graphDir) {
  Problem p(g.graph_size);

  for (const auto &part : g.parts)
    p.addConstraint(new SetStab(part, &p.p_stack));

  if (graphDir == GraphDirected_yes)
    p.addConstraint(new SlowGraph<GraphDirected_yes>(g.edges, &p.p_stack));
  else
    p.addConstraint(new SlowGraph<GraphDirected_no>(g.edges, &p.p_stack));

  SolutionStore ss = doSearch(&p, so);
  return ss.sols();
}
