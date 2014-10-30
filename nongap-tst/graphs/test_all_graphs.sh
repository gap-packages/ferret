#!/bin/bash

set -e

if ! which bliss; then
  echo No bliss, skipping symmetry tests
  exit
fi;

if ! [ -d graph-dist ]; then
  ./download_graphs.sh
fi

for i in $(cat graph-dist/bliss-fast.txt); do
    echo ../scripts/check_graph_syms.sh graph-dist/undirected_dim/$i
done | parallel -j4 | tee graph-results.txt

echo $(wc -l graph-results.txt) graphs
echo $(grep 'Graphs equal' graph-results.txt | wc -l) passes
echo $(grep 'Ferret timeout' graph-results.txt | wc -l) timeouts
echo $(grep 'Graph comparison failed' graph-results.txt | wc -l) failures

exit $(grep 'Graph comparison failed' graph-results.txt | wc -l)
