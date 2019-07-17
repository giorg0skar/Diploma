#!/usr/bin/python

import os
import sys
import io

infile = open(sys.argv[1], "r")
count = 0

for line in infile:
    count += 1

print (sys.argv[1] + " has " + str(count) + " lines")
infile.close()