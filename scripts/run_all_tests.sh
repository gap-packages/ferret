#!/bin/bash
set -eux
set -o pipefail
cd $(dirname $0)
cd ..
. ./ferret.vars
cd YAPB++/tests
./run_tests.sh
cd ../..
for j in "CHECK=0" "CHECK=1"; do
  make $j > /dev/null
(
  for i in $(cd tst; ls *.tst); do
      echo "(cd tst; echo 'Test(\"$i\");' | ${GAPEXEC} -q)"
  done

  if [ "X$VALGRIND" != "X" ]; then
    echo "(cd tst; echo 'Test(\"testvalgrind.tst\");' | $VALGRIND -q --trace-children=yes --suppressions=../gap-suppressions.valgrind ${GAPEXEC} -q)"
  else
    echo "echo Skipping valgrind tests"
  fi;

  echo "(cd tst/graphs; ./test_all_graphs.sh)"
) | parallel -j4
done

if savilerow > /dev/null; then
	(cd tst/symmetry_detect; ./run_savilerow_symmetry_tests.sh )
else
	echo "No savilerow -- skipping tests"
fi
