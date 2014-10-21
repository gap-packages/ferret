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
      echo "(cd tst; echo 'LoadPackage(\"io\"); if Test(\"$i\") = true then IO_exit(0); else IO_exit(1); fi;' | ${GAPEXEC} -q)"
  done | parallel -j4
  wait
  cd ..
done
