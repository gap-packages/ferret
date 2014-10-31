#!/bin/bash
set -eux
set -o pipefail
cd $(dirname $0)
cd ..
. ./ferret.vars
(cd YAPB++/tests && ./run_tests.sh)

for j in "CHECK=1"; do
  make $j > /dev/null
  echo 'Read("tst/testall.g");' | ${GAPEXEC} -q
done
