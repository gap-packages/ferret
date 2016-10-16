#define YAPB_NO_GAP

#include "simple_graph.h"
#include <stdio.h>

#include <map>
#include <set>

ParsedGraph read_saucy_graph(FILE *f) {
  int num_edges, num_cells, num_verts;
  assert(fscanf(f, "%d %d %d", &num_verts, &num_edges, &num_cells) == 3);
  ParsedGraph g(num_verts);
  int prev_cell_start = 0;
  for (int i = 0; i < num_cells - 1; ++i) {
    std::set<int> cell;
    int cell_start;
    assert(fscanf(f, "%d", &cell_start) == 1);
    for (int pos = prev_cell_start; pos < cell_start; ++pos)
      cell.insert(pos + 1);
    g.parts.push_back(cell);
    prev_cell_start = cell_start;
  }

  for (int i = 0; i < num_edges; ++i) {
    int x, y;
    assert(fscanf(f, "%d %d", &x, &y) == 2);
    g.edges[x + 1].push_back(y + 1);
    // g.neighbours(y+1).push_back(x+1);
  }

  return g;
}

ParsedGraph read_dimacs_graph(FILE *fp) {
  FILE *errstr = 0;

  std::map<int, std::set<int>> colours;

  unsigned int nof_vertices;
  unsigned int nof_edges;
  unsigned int line_num = 1;

  const bool verbose = false;
  FILE *const verbstr = stdout;

  /* Read comments and the problem definition line */
  while (1) {
    int c = getc(fp);
    if (c == 'c') {
      /* A comment, ignore the rest of the line */
      while ((c = getc(fp)) != '\n') {
        if (c == EOF) {
          if (errstr)
            fprintf(errstr, "error in line %u: not in DIMACS format\n",
                    line_num);
          abort();
        }
      }
      line_num++;
      continue;
    }
    if (c == 'p') {
      /* The problem definition line */
      if (fscanf(fp, " edge %u %u\n", &nof_vertices, &nof_edges) != 2) {
        if (errstr)
          fprintf(errstr, "error in line %u: not in DIMACS format\n", line_num);
        abort();
      }
      line_num++;
      break;
    }
    if (errstr)
      fprintf(errstr, "error in line %u: not in DIMACS format\n", line_num);
    abort();
  }

  if (nof_vertices <= 0) {
    if (errstr)
      fprintf(errstr, "error: no vertices\n");
    abort();
  }
  if (verbose) {
    fprintf(verbstr, "Instance has %d vertices and %d edges\n", nof_vertices,
            nof_edges);
    fflush(verbstr);
  }

  ParsedGraph g(nof_vertices);

  //
  // Read vertex colors
  //
  if (verbose) {
    fprintf(verbstr, "Reading vertex colors...\n");
    fflush(verbstr);
  }
  while (1) {
    int c = getc(fp);
    if (c != 'n') {
      ungetc(c, fp);
      break;
    }
    ungetc(c, fp);
    unsigned int vertex;
    unsigned int color;
    if (fscanf(fp, "n %u %u\n", &vertex, &color) != 2) {
      if (errstr)
        fprintf(errstr, "error in line %u: not in DIMACS format\n", line_num);
      abort();
    }
    if (!((vertex >= 1) && (vertex <= nof_vertices))) {
      if (errstr)
        fprintf(errstr, "error in line %u: vertex %u not in range [1,...%u]\n",
                line_num, vertex, nof_vertices);
      abort();
    }
    line_num++;
    colours[color].insert(vertex);
  }
  if (verbose) {
    fprintf(verbstr, "Done\n");
    fflush(verbstr);
  }

  //
  // Read edges
  //
  if (verbose) {
    fprintf(verbstr, "Reading edges...\n");
    fflush(verbstr);
  }
  for (unsigned i = 0; i < nof_edges; i++) {
    unsigned int from, to;
    if (fscanf(fp, "e %u %u\n", &from, &to) != 2) {
      if (errstr)
        fprintf(errstr, "error in line %u: not in DIMACS format\n", line_num);
      abort();
    }
    if (not((1 <= from) and (from <= nof_vertices))) {
      if (errstr)
        fprintf(errstr, "error in line %u: vertex %u not in range [1,...%u]\n",
                line_num, from, nof_vertices);
      abort();
    }
    if (not((1 <= to) and (to <= nof_vertices))) {
      if (errstr)
        fprintf(errstr, "error in line %u: vertex %u not in range [1,...%u]\n",
                line_num, to, nof_vertices);
      abort();
    }
    line_num++;
    g.edges[from].push_back(to);
  }
  if (verbose) {
    fprintf(verbstr, "Done\n");
    fflush(verbstr);
  }

  for (auto c : colours) {
    g.parts.push_back(c.second);
  }

  return g;
}
