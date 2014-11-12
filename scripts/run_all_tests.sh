#!/bin/bash
set -eux
set -o pipefail
cd $(dirname $0)
cd ..
. ./ferret.vars

for j in "CHECK=0" "CHECK=1"; do
  scripts/quick_test.sh $j

  (cd nongap-tst/graphs && ./test_all_graphs.sh)
done

if savilerow > /dev/null; then
	(cd tst/symmetry_detect; ./run_savilerow_symmetry_tests.sh )
else
	echo "No savilerow -- skipping tests"
fi
