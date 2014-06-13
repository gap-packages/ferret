#!/bin/bash
set -eux
set -o pipefail
cd $(dirname $0)
cd ..
. ./ferret.vars
cd YAPB++/tests
./run_tests.sh
cd ../..
for j in "CHECK=1"; do
  make $j > /dev/null
  cd tst
  for i in *.tst; do
      echo 'echo '\''Test("'$i'");'\'' | '${GAPEXEC} -q
  done | parallel -j4
  wait
  cd ..
done
