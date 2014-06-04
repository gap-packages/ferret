#include "simple_graph.h"

#include "problem.hpp"
#include "constraints/slowgraph.hpp"
#include "constraints/setstab.hpp"
#include "search.hpp"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>




template<GraphDirected directed>
void SolveGraph(const Graph& g)
{
    Problem p(g.graph_size);

    for(const auto& part: g.parts)
        p.addConstraint(new SetStab(part, &p.p_stack));

    p.addConstraint(new SlowGraph<directed>(g.edges, &p.p_stack));
    SearchOptions so;
    so.only_find_generators = true;
    SolutionStore ss = doSearch(&p, so);

    std::cout << "[";
    for(auto const& sol : ss.sols())
    {
            std::cout << sol.cycle() << ",\n";
    }
    std::cout << "()]\n";
}

int main(int argc, char **argv)
{
    char* filename = 0;
    bool directed = false;
    bool stats = false;

    // formats: 1 - DIMACS, 2 - saucy
    int format = 1;
    for(int i = 1; i < argc; ++i)
    {
        if(argv[i] == std::string("--directed"))
            directed = true;
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


    //if(!directed)
    //    g.make_symmetric();

    g.clean();

    if(directed)
        SolveGraph<GraphDirected_yes>(g);
    else
        SolveGraph<GraphDirected_no>(g);


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
