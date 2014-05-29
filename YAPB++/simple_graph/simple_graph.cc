#include <stdlib.h>
#include <stdio.h>

#include "problem.hpp"
#include "constraints/slowgraph.hpp"
#include "constraints/setstab.hpp"
#include "search.hpp"
#include <iostream>

struct Graph
{
    int graph_size;
    std::vector<std::set<int> > parts;
    vec1<vec1<int> > edges;

    Graph() : graph_size(0)
    { }
};


Graph read_cnf_graph(FILE* f)
{
    Graph g;
    int num_edges, num_cells;
    fscanf(f, "%d %d %d", &(g.graph_size), &num_edges, &num_cells);
    g.edges.resize(g.graph_size);
    int prev_cell_start = 0;
    for(int i = 0; i < num_cells - 1; ++i)
    {
        std::set<int> cell;
        int cell_start;
        fscanf(f, "%d", &cell_start);
        for(int pos = prev_cell_start; pos < cell_start; ++pos)
            cell.insert(pos+1);
        g.parts.push_back(cell);
        prev_cell_start = cell_start;
    }

    for(int i = 0; i < num_edges; ++i)
    {
        int x,y;
        fscanf(f, "%d %d", &x, &y);
        g.edges[x+1].push_back(y+1);
        g.edges[y+1].push_back(x+1);
    }

    return g;
}

void SolveGraph(const Graph& g)
{
    Problem p(g.graph_size);

    for(const auto& part: g.parts)
        p.addConstraint(new SetStab(part, &p.p_stack));

    p.addConstraint(new SlowGraph(g.edges, &p.p_stack));
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

int main(int, char **argv)
{
    FILE* fp = fopen(argv[1], "r");
    if(!fp)
    { perror("Failed to open file:"); }

    SolveGraph(read_cnf_graph(fp));
	return 0;
}
