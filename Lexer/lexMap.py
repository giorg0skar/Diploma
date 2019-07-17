#!/usr/bin/python

import sys
import io
import os

#put the list of keywords in a dictionary

a = {}
a[0]    = "eof"
a[1000] = "and"
a[1001] = "as"
a[1002] = "begin"
a[1003] = "break"
a[1004] = "byte"
a[1005] = "continue"
a[1006] = "decl"
a[1007] = "def"
a[1008] = "elif"
a[1009] = "else"
a[1010] = "end"
a[1011] = "exit"
a[1012] = "false"
a[1013] = "if"
a[1014] = "is"
a[1015] = "int"
a[1016] = "loop"
a[1017] = "not"
a[1018] = "or"
a[1019] = "ref"
a[1020] = "return"
a[1021] = "skip"
a[1022] = "true"
a[1023] = "var"
a[1024] = "intconst"
a[1025] = "char"
a[1026] = "id"
a[1027] = "string"
a[1028] = "greater"
a[1029] = "less"
a[1030] = "greatereq"
a[1031] = "lesseq"
a[1032] = "noteq"
a[1033] = "assign"

# errfile = open(sys.argv[2], "r")
# #To display linenumber, the number needs to be separated by spaces from other words
# #If there is a specific error write it's message after 'error:'

# for line in errfile:
#     if "error:" in line:
#         s = line.split("error:")
#         print("Error is: " + s[1])
#     words = line.split(" ")
#     for char in words:
#         if char.isdigit():
#             linenumber = int(char)
#             print("Error is in line " + str(linenumber))

# errfile.close()
# #if error file was not empty we exit
# if os.stat(sys.argv[2]).st_size != 0:
#     exit(0)

infile = open(sys.argv[1], "r")
token = 0

for line in infile:
    #token_str = infile.readline()
    token_str = line
    token = int(token_str)
    if (token < 1000 and token > 0):
        print (chr(token))
    else:
        print (a[token])

if (token != 0):
    print("A lexical error has been detected. Execution has stopped")

infile.close()