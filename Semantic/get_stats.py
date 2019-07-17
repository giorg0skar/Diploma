#!/usr/bin/python

import os
import sys
import io

infile = open(sys.argv[1], "r")

parent = ""
stats = ""
prev = ""
syntax_error = False

for line in infile:
    if "Index by function name" in line:
        syntax_error = True
        break
    if "error" in line:
        #we remove duplicate spaces from each line
        prev = " ".join(prev.split())
        s = prev.split(" ")
        #print (s)
        parent = s[3]
        stats = s[4]
        break
    prev = line

infile.close()
if (syntax_error):
    infile = open(sys.argv[2], "r")
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

#fname = sys.argv[1] + "_line"
the_line = open("temp_file", "w")
the_line.write(" in line " + st[1])
the_line.close()

if (syntax_error):
    print ("syntax error found in function " + parent +  " at file " + the_file)
else:
    print ("semantic error found in function " + parent + " at file " + the_file)

#infile.close()
