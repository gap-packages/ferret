#ifndef SIMPLE_GRAPH_FDS
#define SIMPLE_GRAPH_FDS

#include "library/vec1.hpp"
#include "library/perm.hpp"
#include "library/graph.hpp"
#include <set>
#include <vector>
#include <stdio.h>

Graph read_saucy_graph(FILE *f);
Graph read_dimacs_graph(FILE *fp);
vec1<Permutation> SolveGraph(const Graph &, GraphDirected graphDir);
#endif
