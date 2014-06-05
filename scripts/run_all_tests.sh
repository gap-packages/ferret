#!/bin/bash
set -e
set -u
cd $(dirname $0)
cd ..
. ./ferret.vars
cd YAPB++/tests
./run_tests.sh
cd ../..
for j in "CHECK=0" "CHECK=1"; do
  make $j > /dev/null
  cd tst
  for i in *.tst; do
      echo 'echo '\''Test("'$i'");'\'' | '${GAPEXEC}
  done | parallel -j4
  wait
  cd ..
done

if [ "X$VALGRIND" != "X" ]; then
  echo make valgrind
  make "CHECK=1" > /dev/null
  cd tst
  echo 'Test("testvalgrind.tst");' | $VALGRIND -q --trace-children=yes --suppressions=../gap-suppressions.valgrind ${GAPEXEC} -b
else
  echo Skipping valgrind tests
fi;
