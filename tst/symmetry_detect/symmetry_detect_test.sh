#!/usr/bin/env bash

./jsonToGap.py <(symmetry_detect $1 -q) $2 | gap.sh -q | grep 'true' > /dev/null

status=$?
if [ $status -eq 0 ] ; then
	echo "symmetry_detect passed for file $1.  Compared against file $2."
else
	echo "failed test with input file $1.  Compared against file $2."
fi
exit $status 