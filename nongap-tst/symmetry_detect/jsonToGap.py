#!/usr/bin/env python
import sys
import json

def parseFile(fileName):
    syms = None
    json_str = None
    try:
        with open(fileName) as f:
            json_str = f.read()
        syms = json.loads(json_str)
    except:
        sys.stderr.write(sys.exc_info().__str__() + "\n")
        if json_str != None:
            sys.stderr.write("Input file " + fileName + ":\n" + json_str)
            
    if  not isinstance(syms, list):
        sys.stderr.write("expected JSON list at top level of file:" + fileName + "\n")
        sys.exit(2);
    return syms


def removeDuplicates(syms):
    if len(syms) == 0:
        return
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
    buffer = [];
    firstPerm = True;
    buffer.append("[")
    for sym in syms:
        p = toPermutation(sym)
        l = toListNotation(p)
        l = l[1:len(l)]
        if not firstPerm:
            buffer.append(",")
        else:
            firstPerm = False;
        buffer.append(str(l))
    buffer.append("]")
    return "".join(buffer)



def convert(fileName):
    syms = parseFile(fileName)
    if len(syms) == 0:
        return "[]"
    removeDuplicates(syms)
    enumerateValues(syms)
    return jsonToListNotation(syms)


if len(sys.argv) < 3:
    print "usage: ", sys.argv[0], "json_file_1 json_file_2"
    sys.exit(1)

var1 =  convert(sys.argv[1])
var2 =  convert(sys.argv[2])
if var1 == "[]":
    temp = (var2 == "[]").__str__().lower()
    print "Print (" + temp + ");"
    sys.exit(0)
elif var2 == "[]":
    print "Print (false);"
    sys.exit(0)

        
    

#build gap string
buffer = []
buffer.append("a := ")
buffer.append(var1)
buffer.append(";;\n")
buffer.append("b := ")
buffer.append(var2)
buffer.append(";;\n")
buffer.append("""ga := Group(List(a, PermList));;

gb := Group(List(b, PermList));;

Print(ga = gb, "\\n");;""")


print "".join(buffer)