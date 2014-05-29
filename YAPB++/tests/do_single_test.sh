#!/bin/bash
. ../../ferret.vars

$CXX  $1 -I ../source $DEBUGFLAGS -Wall -Wextra -g -o $1.exe && $VALGRIND ./$1.exe && rm $1.exe

