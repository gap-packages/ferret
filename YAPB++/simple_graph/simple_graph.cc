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

        for(int i = 1; i <= graph_size; ++i)
        {
            std::set<int> e(edges[i].begin(), edges[i].end());
            edges[i] = vec1<int>(e.begin(), e.end());
        }
    }
};


Graph read_saucy_graph(FILE* f)
{
    int num_edges, num_cells, num_verts;
    fscanf(f, "%d %d %d", &num_verts, &num_edges, &num_cells);
    Graph g(num_verts);
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
        //g.edges[y+1].push_back(x+1);
    }

    return g;
}

Graph read_dimacs_graph(FILE* fp)
{
    FILE* errstr = 0;


  std::map<int, std::set<int> > colours;

  unsigned int nof_vertices;
  unsigned int nof_edges;
  unsigned int line_num = 1;

  const bool verbose = false;
  FILE* const verbstr = stdout;

  /* Read comments and the problem definition line */
  while(1)
    {
      int c = getc(fp);
      if(c == 'c')
        {
          /* A comment, ignore the rest of the line */
          while((c = getc(fp)) != '\n')
            {
              if(c == EOF) {
                if(errstr)
                  fprintf(errstr, "error in line %u: not in DIMACS format\n",
                          line_num);
                        abort();
              }
            }
          line_num++;
          continue;
        }
      if(c == 'p')
        {
          /* The problem definition line */
          if(fscanf(fp, " edge %u %u\n", &nof_vertices, &nof_edges) != 2)
            {
              if(errstr)
                fprintf(errstr, "error in line %u: not in DIMACS format\n",
                        line_num);
                      abort();
            }
          line_num++;
          break;
        }
      if(errstr)
        fprintf(errstr, "error in line %u: not in DIMACS format\n", line_num);
              abort();
    }

  if(nof_vertices <= 0)
    {
      if(errstr)
        fprintf(errstr, "error: no vertices\n");
              abort();
    }
  if(verbose)
    {
      fprintf(verbstr, "Instance has %d vertices and %d edges\n",
              nof_vertices, nof_edges);
      fflush(verbstr);
    }

    Graph g(nof_vertices);


  //
  // Read vertex colors
  //
  if(verbose)
    {
      fprintf(verbstr, "Reading vertex colors...\n");
      fflush(verbstr);
    }
  while(1)
    {
      int c = getc(fp);
      if(c != 'n')
        {
          ungetc(c, fp);
          break;
        }
      ungetc(c, fp);
      unsigned int vertex;
      unsigned int color;
      if(fscanf(fp, "n %u %u\n", &vertex, &color) != 2)
        {
          if(errstr)
            fprintf(errstr, "error in line %u: not in DIMACS format\n",
                    line_num);
                  abort();
        }
      if(!((vertex >= 1) && (vertex <= nof_vertices)))
        {
          if(errstr)
            fprintf(errstr,
                    "error in line %u: vertex %u not in range [1,...%u]\n",
                    line_num, vertex, nof_vertices);
                  abort();
        }
      line_num++;
      colours[color].insert(vertex);
    }
  if(verbose)
    {
      fprintf(verbstr, "Done\n");
      fflush(verbstr);
    }

  //
  // Read edges
  //
  if(verbose)
    {
      fprintf(verbstr, "Reading edges...\n");
      fflush(verbstr);
    }
  for(unsigned i = 0; i < nof_edges; i++)
    {
      unsigned int from, to;
      if(fscanf(fp, "e %u %u\n", &from, &to) != 2)
        {
          if(errstr)
            fprintf(errstr, "error in line %u: not in DIMACS format\n",
                    line_num);
                  abort();
        }
      if(not((1 <= from) and (from <= nof_vertices)))
        {
          if(errstr)
            fprintf(errstr,
                    "error in line %u: vertex %u not in range [1,...%u]\n",
                    line_num, from, nof_vertices);
                  abort();
        }
      if(not((1 <= to) and (to <= nof_vertices)))
        {
          if(errstr)
            fprintf(errstr,
                    "error in line %u: vertex %u not in range [1,...%u]\n",
                    line_num, to, nof_vertices);
                  abort();
        }
      line_num++;
      g.edges[from].push_back(to);
    }
  if(verbose)
    {
      fprintf(verbstr, "Done\n");
      fflush(verbstr);
    }


    for(auto c : colours)
    {
        g.parts.push_back(c.second);
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

int main(int argc, char **argv)
{
    char* filename = 0;
    bool directed = false;

    // formats: 1 - DIMACS, 2 - saucy
    int format = 1;
    for(int i = 1; i < argc; ++i)
    {
        if(argv[i] == std::string("--directed"))
            directed = true;
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


    if(!directed)
        g.make_symmetric();

    SolveGraph(g);
    return 0;
}
