#!/usr/bin/python

import sys
import io
import os
import random

print("Generating test case files: ")

#put the list of keywords in a list

a = []
#a[0] = "eof"
a.append("and")
a.append("as")
a.append("begin")
a.append("break")
a.append("byte")
a.append("continue")
a.append("decl")
a.append("def")
a.append("elif")
a.append("else")
a.append("end")
a.append("exit")
a.append("false")
a.append("if")
a.append("is")
a.append("int")
a.append("loop")
a.append("not")
a.append("or")
a.append("ref")
a.append("return")
a.append("skip")
a.append("true")
a.append("var")
a.append("intconst")
a.append("char")
a.append("id")
a.append("string")
a.append("greater")
a.append("less")
a.append("greatereq")
a.append("lesseq")
a.append("noteq")
a.append("assign")

#iterate through the list and create a separate file for each keyword

for i in range(len(a)):
    testfile = open("tests/" + a[i] + ".dna", "w")
    testfile.write(a[i])
    testfile.close()

testfile = open("tests/line_comment.dna", "w")
testfile.write("#i can write whatever i want")
testfile.close()
a.append("line_comment")

testfile = open("tests/multiple_line_comment.dna", "w")
testfile.write("(*(*i can write whatever i want\n")
testfile.write("now in multiple*)\n")
testfile.write(" lines*)")
testfile.close()
a.append("multiple_line_comment")

testfile = open("tests/string.dna", "w")
testfile.write("\"abcdefghigk\"")
testfile.close()
a.append("string")

testfile = open("tests/single_char.dna", "w")
testfile.write("\'c\'")
testfile.close()
a.append("single_char")

testfile = open("tests/ascii.dna", "w")
testfile.write("\\x0f")
testfile.close()
a.append("ascii")

# for test in a:
#     inputname = "tests/" + test + ".dna"
#     print ("Running test: " + test)
#     print ("----------------------")
#     os.system("./check_lexer.sh " + inputname)

for x in range(10):
    name = "tests/" + str(x) + ".dna"
    testfile = open(name, "w")
    for y in range(10):
        index = random.randint(0,38)
        rfile = open("tests/" + a[index] + ".dna", "r")
        contents = rfile.readline()
        testfile.write(contents)
        testfile.write(" ")
        rfile.close()
    testfile.close()
