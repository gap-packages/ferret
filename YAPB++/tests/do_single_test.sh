#!/bin/bash
. ../../ferret.vars

${CXX:-c++} -std=c++14 $1 -I ../source -I ../../gap_cpp_headers -DYAPB_NO_GAP $DEBUGFLAGS -g -DDEBUG_LEVEL=2 -Wall -Wextra -o $1.exe && $VALGRIND ./$1.exe && rm -r $1.exe*
