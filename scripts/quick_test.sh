#!/bin/bash
set -e
cd $(dirname $0)
cd ..
. ./ferret.vars
cd YAPB++/tests
./run_tests.sh
cd ../..
for j in ""; do
  echo make $i
  make $i > /dev/null
  cd tst
  for i in *.tst; do
      echo 'echo '\''Test("'$i'");'\'' | '${GAPEXEC}
  done | parallel -j4
  wait
  cd ..
done
