#!/usr/bin/env bash

 $(dirname -- "$0")/check_groups_equal.sh \
   <( $(dirname -- "$0")/../simple_graph $1) \
   <( $(dirname -- "$0")/../old_store/saucy-0.3/gapsaucy.sh $1)
