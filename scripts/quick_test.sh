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
echo 'Read("tst/testall.g");' | ${GAPEXEC} -q

if [ "X$VALGRIND" != "X" ]; then
  (cd tst && echo 'LoadPackage(\"io\"); if Test(\"testvalgrind.tst\") = true then IO_exit(0); else IO_exit(1); fi;' | $VALGRIND -q --trace-children=yes --suppressions=../scripts/gap-suppressions.valgrind ${GAPEXEC} -q)
else
  echo "echo Skipping valgrind tests"
fi;
