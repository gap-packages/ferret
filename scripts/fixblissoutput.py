#!/usr/bin/env python3
import fileinput
import re

pattern = re.compile('Generator:(.*)')
print("[")
for line in fileinput.input():
    m = pattern.search(line)
    if m:
        print(m.group(1), ",")
print("()]")
