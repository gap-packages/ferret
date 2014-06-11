#!/usr/bin/env python
import sys
import json
#validate arguments
if len(sys.argv) < 2:
    print "usage: ", sys.argv[0], "path_to_json_file"
    sys.exit(1)


syms = json.load(open(sys.argv[1], "r"))
if  not isinstance(syms, list):
    print "expected JSON list object."
    sys.exit(2);

#remove duplicates 
syms.sort()
sym = syms[0]
i = 1
while i < len(syms):
    if sym == syms[i]:
        del syms[i]
    else:
        sym = syms[i]


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



p = toPermutation(syms[0])
l = toListNotation(p)
print p
print l

print 