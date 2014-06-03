#!/bin/bash
for i in *.cc; do
	echo ./do_single_test.sh $i
done | parallel -j4
