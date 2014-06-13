#!/usr/bin/env bash

if ! $(dirname -- "$0")/check_groups_equal.sh \
   <( $(dirname -- "$0")/../simple_graph $1) \
   <( $(dirname -- "$0")/gapbliss.sh $1); then
   echo Graph comparison failed: $1
   exit 1
fi
