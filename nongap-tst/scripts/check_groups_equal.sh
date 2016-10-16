#!/usr/bin/env bash
(echo 'LoadPackage("IO");;'
 echo 'g1 := Group(EvalString(IO_ReadUntilEOF(IO_File("'$1'","r"))));;'
 echo 'g2 := Group(EvalString(IO_ReadUntilEOF(IO_File("'$2'","r"))));;'
 echo 'if g1 = g2 then'
 echo 'Print("\ntrue\n");'
 echo 'else'
 echo 'Print("false (sizes:", Size(g1), ",", Size(g2),")");'
 echo 'fi;') | ${GAPEXEC} -q | grep '^true$' > /dev/null
