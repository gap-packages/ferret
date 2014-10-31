#!/bin/bash
set -eux
set -o pipefail
cd $(dirname $0)
cd ..
. ./ferret.vars
(cd YAPB++/tests && ./run_tests.sh)

for j in "CHECK=0" "CHECK=1"; do
  make all $j > /dev/null
  echo 'Read("tst/testall.g");' | ${GAPEXEC} -q

  if [ "X$VALGRIND" != "X" ]; then
    (cd tst && echo 'LoadPackage(\"io\"); if Test(\"testvalgrind.tst\") = true then IO_exit(0); else IO_exit(1); fi;' | $VALGRIND -q --trace-children=yes --suppressions=../scripts/gap-suppressions.valgrind ${GAPEXEC} -q)
  else
    echo "echo Skipping valgrind tests"
  fi;

  (cd nongap-tst/graphs && ./test_all_graphs.sh)
done

if savilerow > /dev/null; then
	(cd tst/symmetry_detect; ./run_savilerow_symmetry_tests.sh )
else
	echo "No savilerow -- skipping tests"
fi
