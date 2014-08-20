#include "simple_graph.h"

#include <iostream>
#include <stdlib.h>
#include <stdio.h>

#include "library/stats.hpp"

void outputGraph(const Graph& g, GraphDirected directed)
{
    auto sols = SolveGraph(g, directed);

    std::cout << "[";
    for(auto const& sol : sols)
    {
            std::cout << sol.cycle() << ",\n";
    }
    std::cout << "()]\n";
}

int main(int argc, char **argv)
{
    char* filename = 0;
    GraphDirected directed = GraphDirected_no;
    bool stats = false;

    // formats: 1 - DIMACS, 2 - saucy
    int format = 1;
    for(int i = 1; i < argc; ++i)
    {
        if(argv[i] == std::string("--directed"))
            directed = GraphDirected_yes;
        else if(argv[i] == std::string("--stats"))
            stats = true;
        else if(argv[i] == std::string("--saucy"))
            format = 2;
        else
            filename = argv[i];
    }

    FILE* fp = fopen(filename, "r");

    if(!fp)
    { perror("Failed to open file:"); }

    Graph g;
    switch(format)
    {
        case 1: g = read_dimacs_graph(fp); break;
        case 2: g = read_saucy_graph(fp); break;
        default: abort();

    }

    g.clean();

    outputGraph(g, directed);


    if(stats) {
        std::cerr << "Nodes: " << Stats::container().node_count
                  << " Bad leaves: " << Stats::container().bad_leaves
                  << " Bad internal: "
                    << Stats::container(). bad_internal_nodes << "\n";
        for(auto p : Stats::container().getConstraintCalls())
        {
            std::cerr << p.first << ": " << p.second << "\n";
        }
    }
    return 0;
}
