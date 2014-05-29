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

#if 0
#include "gason/gason.h"

// Super naughty!
#include "gason/gason.cpp"


Graph json_parser(FILE fp)
{
    char *source = nullptr;
    size_t source_size = 0;
    size_t buffer_size = 0;

    while (!feof(fp))
    {
        if (source_size + 1 >= buffer_size)
        {
            buffer_size = buffer_size < BUFSIZ ? BUFSIZ : buffer_size * 2;
            source = (char *)realloc(source, buffer_size);
        }
        source_size += fread(source + source_size, 1, buffer_size - source_size - 1, fp);
    }
    fclose(fp);
    source[source_size] = 0;

    char *endptr = (char*)1;
    JsonValue value;
    JsonAllocator allocator;

    JsonParseStatus status = gason_jsonParse(source, &endptr, &value, allocator);
    if (status != JSON_PARSE_OK)
    {
        gason_print_error(filename, status, endptr, source, source_size);
        exit(EXIT_FAILURE);
    }

    return read_json_graph(value);
}

Graph read_json_graph(JsonValue o)
{
    Graph g;
    if(o.getTag() != JSON_TAG_ARRAY)
    {
        std::cerr << "Must start with object\n";
        exit(1);
    }

    for(auto mem : o)
    {
        if(mem->key == std::string("vertices"))
        {
            g.graph_size = (int)(mem->value.toNumber());
        }
        else if(mem->key == std::string("parts"))
        {
            for(auto cell : mem->value)
            {
                std::set<int> part_cell;
                for(auto v : cell->value)
                    part_cell.insert((int)(v->value.toNumber()));
                g.parts.push_back(part_cell);
            }
        }
        else if(mem->key == std::string("edges"))
        {
            for(auto edgelist : mem->value)
            {
                vec1<int> vertex_edgelist;
                for(auto e : edgelist->value)
                    vertex_edgelist.push_back((int)(e->value.toNumber()));
                g.edges.push_back(vertex_edgelist);
            }
        }
        else
        {
            std::cerr << "Do not understand " << mem->key << "\n";
            exit(1);
        }
    }
}
#endif


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
