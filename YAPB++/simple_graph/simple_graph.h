#ifndef SIMPLE_GRAPH_FDS
#define SIMPLE_GRAPH_FDS

#include "library/vec1.hpp"
#include "library/perm.hpp"
#include "library/graph.hpp"
#include "search/search_options.hpp"
#include "library/graph.hpp"

#include <set>
#include <vector>
#include <stdio.h>

ParsedGraph read_saucy_graph(FILE *f);
ParsedGraph read_dimacs_graph(FILE *fp);
vec1<Permutation> SolveGraph(const ParsedGraph &, SearchOptions so, GraphConfig gc, GraphDirected graphDir);
void solveJsonGraph(const std::string& filename, SearchOptions so, GraphConfig gc);

#endif
