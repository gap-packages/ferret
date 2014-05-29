#!/usr/bin/env python

import re
import fileinput

# A simple wrapper to push back onto a python iterator 
class pushback_wrapper(object):
    def __init__(self, it):
        self.it = it
        self.pushed_back = []
        
    def __iter__(self):
        return self
        
    def next(self):
        try:
            return self.pushed_back.pop()
        except IndexError:
            return self.it.next()
        
    def pushback(self, item):
        self.pushed_back.append(item)

def testLine(reg, it):
	line = it.next()
	if(re.search(reg, line)):
		return True
	else:
		debug_info("skipping(1) '" + line + "'")
		return False

def debug_info(string):
	pass
	# print(string)

def readNode(it):
	node = { 'children': [] }
	while True:
		line = it.next()
		trysol = re.search("Sol:(.*)", line)
		branchnode = re.search("branch on: (\d*)", line)

		if re.search("pushWorld", line):
			node['children'].append(readNode(it))
		elif re.search("popWorld", line) or re.search("Node count", line):
			return node
		elif trysol:
			node["solution"] = eval(trysol.group(1))
		elif branchnode:
			node["name"] = eval(branchnode.group(1))
		else:
			debug_info("skipping(3) '" + line + "'")

	
def readRBase(it):
	RBase = [];

	while True:
		line = it.next()
		if re.search('Finish RBase', line):
			return RBase
		match = re.search('RBase:Level (\d*) : (\d*), location (\d*), in cell (\d*), size (\d*)', line)
		if match:
			RBase.append({'cell': match.group(4), 'size': match.group(5)})
		else:
			debug_info("skipping(2) '" + line + "'")




iterator = pushback_wrapper(fileinput.input())

rBase = readRBase(iterator)


node = readNode(iterator)
print([rBase, node])

