#!/usr/bin/python

import os
import sys
import io

# infile = open(sys.argv[1], "r")

parent = ""
stats = ""

# for line in infile:
#     if "Index by function name" in line:
#         not_found = True
#         break
#     if "error" in line:
#         #we remove duplicate spaces from each line
#         prev = " ".join(prev.split())
#         s = prev.split(" ")
#         print (s)
#         parent = s[3]
#         stats = s[4]
#         break
#     prev = line

# infile.close()

infile = open(sys.argv[1], "r")
prev = ""
for line in infile:
    if "yyerror" in line:
        prev = " ".join(prev.split())
        s = prev.split(" ")
        #print (s)
        parent = s[3]
        stats = s[4]
        break
    prev = line
infile.close()

#print ("Parent function is " + parent)
st = stats.split(':')
#l =  [ int(s) for s in st.split() if s.isdigit() ]
the_file = (st[0])[1:]
print ("lexical error found in function " + parent +  " at file " + the_file + " in line " + st[1])

#infile.close()
