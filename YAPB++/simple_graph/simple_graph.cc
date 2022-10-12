#define YAPB_NO_GAP

#include "simple_graph.h"

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <chrono>
#include "library/stats.hpp"
#include "search/search_options.hpp"

void outputGraph(const ParsedGraph& g, SearchOptions so, GraphConfig gc,
                 GraphDirected directed, bool stats)
{
    auto sols = SolveGraph(g, so, gc, directed);

    std::cout << "[";
    for(auto const& sol : sols)
    {
            std::cout << sol.cycle() << ",\n";
    }
    std::cout << "()]\n";
    if(stats)
    {
      std::cout << "Generators: " << sols.size() << "\n";
    }
}

void print_usage_instructions()
{
    std::cout <<
    " A simple graph symmetry detector\n"
    " Usage:\n"
    "  ./symmetry_detect [--stats] [--directed] [--order [random|scf|ordered]]\n"
    "                     [--find [generators|all]]\n"
    "                     --saucy|--dimacs|--json <filename>\n"
    " --stats    : Print extra stats\n"
    " --directed : Graph is directed (ignored with --json)\n"
    " --order    : The order in which search should be performed:\n"
    "       random  : Choose search order randomly\n"
    "       scf     : Choose smallest cell to branch on\n"
    "       directed: Choose cell which is most connected in the graph(default)\n"
    "       ordered : Choose cells in order of input\n"
    " --find     : Which set of permutations should be produced:\n"
    "       generators : Generators of the symmetry group (default)\n"
    "       all        : All members of the symmetry group (can be VERY large)\n"
    " --saucy    : Accept graph in saucy format\n"
    " --dimacs   : Accept graph in dimacs format\n"
    " --json     : Accept an AST in json format (see in-depth docs)\n"
    " <filename> : Filename of graph\n"
    " --startpathlength <num> : Length of paths to consider at root node (default 1)"
    " --normalpathlength <num> : Length of paths to consider at all other nodes (default 1)";
}


int main(int argc, char **argv)
{
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point endTime;
    startTime = std::chrono::high_resolution_clock::now();
    char* filename = 0;
    GraphDirected directed = GraphDirected_no;
    bool stats = false;
    SearchOptions so;
    so.heuristic = Heuristic::adviseHeuristic();
    // formats: 1 - DIMACS, 2 - saucy
    int format = 1;
    GraphConfig gc;
    for(int i = 1; i < argc; ++i)
    {
        if(argv[i] == std::string("--directed"))
            directed = GraphDirected_yes;
        else if(argv[i] == std::string("--stats"))
            stats = true;
        else if(argv[i] == std::string("--dimacs"))
            format = 1;
        else if(argv[i] == std::string("--saucy"))
            format = 2;
        else if(argv[i] == std::string("--json"))
        {
            format = 3;
            directed = GraphDirected_yes;
        }
        else if(argv[i] == std::string("--order"))
        {
            i++;
            if(argv[i] == std::string("random"))
                {
                    so.heuristic = Heuristic::randomHeuristic();
                }
            else if(argv[i] == std::string("scf"))
            {
                so.heuristic = Heuristic::scfHeuristic();
            }
            else if(argv[i] == std::string("ordered"))
            {
                so.heuristic = Heuristic::orderHeuristic();
            }
            else if(argv[i] == std::string("directed"))
            {
                so.heuristic = Heuristic::adviseHeuristic();
            }
            else
                {
                    std::cerr << "Do not understand order '" << argv[i] << "'\n";
                    exit(1);
                }
        }
        else if(argv[i] == std::string("--find"))
        {
            i++;
            if(argv[i] == std::string("generators"))
                so.only_find_generators = true;
            else if(argv[i] == std::string("all"))
                so.only_find_generators = false;
            else
                {
                    std::cerr << "Do not understand --find argument '" << argv[i] << "'\n";
                    exit(1);
                }
        }
        else if(argv[i] == std::string("--startpathlength"))
        {
            i++;
            gc.start_path_length = atoi(argv[i]);
            if(gc.start_path_length <= 0) {
                std::cerr << "Invalid startpathlength\n";
                exit(1);
            }
        }
        else if(argv[i] == std::string("--normalpathlength"))
        {
            i++;
            gc.normal_path_length = atoi(argv[i]);
            if(gc.normal_path_length <= 0) {
                std::cerr << "Invalid normalpathlength\n";
                exit(1);
            }
        }
        else if(argv[i] == std::string("-h") || argv[i] == std::string("-help") || argv[i] == std::string("--help"))
        {
            print_usage_instructions();
            exit(0);
        }
        else
        {
            if(filename == 0)
                filename = argv[i];
            else
            {
                std::cerr << "Only one filename allowed\n";
                exit(1);
            }
        }
    }

    if(filename == 0)
    {
        std::cerr << "Error: No filename" << std::endl;
        print_usage_instructions();
        exit(1);
    }
    if(format == 3)
    {
        solveJsonGraph(filename, so, gc);
    }
    else
    {
    FILE* fp = fopen(filename, "r");

    if(!fp)
    { 
        std::cerr << "Could not open file '" << filename << "'\n";
        perror("Error: "); 
    }

    ParsedGraph g;
    switch(format)
    {
        case 1: g = read_dimacs_graph(fp); break;
        case 2: g = read_saucy_graph(fp); break;
        default: abort();

    }

    g.clean();

    outputGraph(g, so, gc, directed, stats);
}

    if(stats) {
      endTime = std::chrono::high_resolution_clock::now();
      int64_t timeTaken =
        std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
      timeTaken /= 1000;
      std::cout << "time taken (ms): " << timeTaken << std::endl;
      std::cout << "Nodes: " << Stats::container().node_count << "\n"
                << "Bad leaves: " << Stats::container().bad_leaves << "\n"
                  << "Bad internal: "
                    << Stats::container(). bad_internal_nodes << "\n";
        for(auto p : Stats::container().getConstraintCalls())
        {
            std::cout << p.first << ": " << p.second << "\n";
        }
    }
    return 0;
}
