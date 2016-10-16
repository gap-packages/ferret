#!/usr/bin/env bash
# Magic for tmpdir

mytmpdir=`mktemp -d 2>/dev/null || mktemp -d -t 'mytmpdir'`

(ulimit -t 30; $(dirname -- "$0")/../../symmetry_detect --dimacs $1 ) > $mytmpdir/ferret 2>&1

ret=$?

if [ "$ret" -ne "0" ]; then
  echo "Ferret timeout/fail: "$ret" "$1
  exit 0
fi

$(dirname -- "$0")/gapbliss.sh $1 > $mytmpdir/bliss

if ! $(dirname -- "$0")/check_groups_equal.sh \
   $mytmpdir/ferret $mytmpdir/bliss ; then
   echo Graph comparison failed: $1
   exit 1
else
   echo Graphs equal: $1
   exit 0
fi
