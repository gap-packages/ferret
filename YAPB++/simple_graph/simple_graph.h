#ifndef SIMPLE_GRAPH_FDS
#define SIMPLE_GRAPH_FDS

#include "library/vec1.hpp"
#include "library/perm.hpp"
#include <set>
#include <vector>
#include <stdio.h>

struct Graph
{
    int graph_size;
    std::vector<std::set<int> > parts;
    vec1<vec1<int> > edges;

    Graph(int _size = 0) : graph_size(_size), edges(_size)
    { }

    void make_symmetric()
    {
        for(int i = 1; i <= graph_size; ++i)
        {
            for(int j = 1; j <= edges[i].size(); ++j)
            {
                edges[edges[i][j]].push_back(i);
            }
        }
    }

    void clean()
    {
        for(int i = 1; i <= graph_size; ++i)
        {
            std::set<int> e(edges[i].begin(), edges[i].end());
            edges[i] = vec1<int>(e.begin(), e.end());
        }
    }
};

Graph read_saucy_graph(FILE* f);
Graph read_dimacs_graph(FILE* fp);
vec1<Permutation> SolveGraph(const Graph&);
#endif
