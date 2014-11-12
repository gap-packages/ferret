#!/bin/bash
set -eux
set -o pipefail
cd $(dirname $0)
cd ..
. ./ferret.vars

if [ $# -eq 0 ]
then
  MAKEFLAGS="CHECK=1"
else
  MAKEFLAGS="$*"
fi

(cd YAPB++/tests && ./run_tests.sh)


make ${MAKEFLAGS} > /dev/null
echo 'Read("tst/testall.g");' | ${GAPEXEC} -A -q

if [ "X$VALGRIND" != "X" ]; then
  $VALGRIND -q --trace-children=yes --suppressions=scripts/gap-suppressions.valgrind ${GAPEXEC} -A -q tst/testvalgrind.g
else
  echo "echo Skipping valgrind tests"
fi;
