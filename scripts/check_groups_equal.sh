#!/usr/bin/env bash

(echo 'LoadPackage("IO");;'
 echo 'g1 := Group(EvalString(IO_ReadUntilEOF(IO_File("'$1'","r"))));;'
 echo 'g2 := Group(EvalString(IO_ReadUntilEOF(IO_File("'$2'","r"))));;'
 echo 'g1 = g2;') | gap.sh -q
