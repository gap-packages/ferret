#!/bin/bash
. ../../ferret.vars

$CXX $1 -I ../source $DEBUGFLAGS -Wall -Wextra -std=gnu++11  -o $1.exe && $VALGRIND ./$1.exe && rm -r $1.exe*
