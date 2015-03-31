#ifndef SIMPLE_GRAPH_FDS
#define SIMPLE_GRAPH_FDS

#include "vec1.hpp"
#include "library/perm.hpp"
#include "library/graph.hpp"
#include "search_options.hpp"

#include <set>
#include <vector>
#include <stdio.h>

Graph read_saucy_graph(FILE *f);
Graph read_dimacs_graph(FILE *fp);
vec1<Permutation> SolveGraph(const Graph &, SearchOptions so, GraphDirected graphDir);
void solveJsonGraph(const std::string& filename, SearchOptions so);

#endif
