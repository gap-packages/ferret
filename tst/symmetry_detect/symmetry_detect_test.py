#!/usr/bin/env python
import sys
import json
#validate arguments

def parseFile(fileName):
    syms = json.load(open(sys.argv[1], "r"))
    if  not isinstance(syms, list):
        print "expected JSON list at top level of file:", fileName
        sys.exit(2);
    return syms


def removeDuplicates(syms):
    syms.sort()
    sym = syms[0]
    i = 1
    while i < len(syms):
        if sym == syms[i]:
            del syms[i]
        else:
            sym = syms[i]
            i += 1



#enumerate all key and values 

#function used to map unique number to each string passed in
uniqueIndex = 1
values = dict()
def addUniqueValueTo(value):
    global uniqueIndex
    global values
    if value not  in values:
        values[value] = uniqueIndex
        uniqueIndex += 1




def enumerateValues(syms):
    for sym in syms:
        for (k,v) in sym.iteritems():
            addUniqueValueTo(k)
            addUniqueValueTo(v)



#returns list of int tuples (a permutation)
def toPermutation(jsonSym):
    perm = list()
    for (k,v) in jsonSym.iteritems():
        perm.append((values[k], values[v]))
    return perm



def extendList(listIn, newSize):
    oldSize = len(listIn)
    if newSize < oldSize:
        return 
    for x in range(oldSize, newSize + 1):
        listIn.append(x);




def toListNotation(perm):
    outList = list()
    for (a,b) in perm:
        extendList(outList,a)
        outList[a] = b
    return outList


def jsonToListNotation(syms):
    firstPerm = False;
    print "["
    for sym in syms:
        p = toPermutation(sym)
        l = toListNotation(p)
        l = l[1:len(l)]
        if firstPerm:
            print l
            firstPerm = false
        else:
            print str(l) + ","
    print "]"



def convert(fileName):
    syms = parseFile(fileName)
    removeDuplicates(syms)
    enumerateValues(syms)
    jsonToListNotation(syms)


if len(sys.argv) < 3:
    print "usage: ", sys.argv[0], "json_file_1 json_file_2"
    sys.exit(1)

convert(sys.argv[1])
print ""
convert(sys.argv[2])