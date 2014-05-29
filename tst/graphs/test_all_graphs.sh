#!/bin/bash

for i in *.graph; do
    echo ../../scripts/check_graph_syms.sh $i
done | parallel -j4
