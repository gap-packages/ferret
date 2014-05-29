#!/bin/bash
set -e
cd ..
. ./ferret.vars
cd YAPB++/tests
./run_tests.sh
cd ../..
for i in ""; do
  echo make $i
  make $i > /dev/null
  cd tst
  echo 'Test("testall.tst");' | ${GAPEXEC} &
  echo 'Test("testslow.tst");' | ${GAPEXEC} &
  echo 'Test("test_minimage.tst");' | ${GAPEXEC}
  wait
  cd ..
done
