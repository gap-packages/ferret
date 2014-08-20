#ifndef _GRAPH_HPP_FDAJIOQ
#define _GRAPH_HPP_FDAJIOQ

#include "vec1.hpp"

// This file stores some simple generic graph related structures

enum GraphDirected
{ GraphDirected_no = 0, GraphDirected_yes = 1};

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

#endif
