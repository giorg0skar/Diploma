#!/usr/bin/python

import sys
import io
from os import system

the_args = sys.argv
path = the_args[1]

#we look for the .l file (commonly lexer.l)
lex_file = path + "/lexer.l"

list_of_headers = []

to_read = open(lex_file, "r")
for line in to_read:
    substring = "#include"
    #find all lines who include files
    if substring in line:
        words = line.split()
        if (words[0][:2] == "//" or words[0][:2] == "/*"):
            continue
        for header in words:
            if (header[0] == '\"'):
                list_of_headers.append(header[1:-1])

to_read.close()

print (list_of_headers)

example = open("sample", "r")
main_func = example.read()
example.close()

#print(main_func)
to_read = open(lex_file, "r")
contents = to_read.read()
to_read.close()

#system("cd " + path)

to_write = open("trylexer.l", "w")
to_write.write(contents)
to_write.close()

to_append = open("trylexer.l", "a")
to_append.write("\n")
to_append.write(main_func)
to_append.close()

to_execute = ""
for h in list_of_headers:
    to_execute += path + "/" + h + " "
    system("cp " + path + "/" + h + " .")

# print(to_execute)
# exit(0)

system("flex -s -o trylexer.c trylexer.l")
# system("bison -dv -o parser.c parser.y")
system("g++ -Wall -c trylexer.c " + to_execute)
system("g++ -Wall -o trylexer trylexer.o symbol.o -lfl")

system("./trylexer < tests/1.dna")

#system("cd ~/Documents/Diplomatiki/Lexer")
