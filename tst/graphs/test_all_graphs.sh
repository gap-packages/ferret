#!/bin/bash

set -e

if ! [ -d graph-dist ]; then
  ./download_graphs.sh
fi

for i in $(cat graph-dist/ferret-fast.txt); do
    echo ../../scripts/check_graph_syms.sh graph-dist/undirected_dim/$i
done | parallel -j4
