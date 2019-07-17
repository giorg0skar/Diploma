#!/usr/bin/python

import sys
import io
from os import listdir, system
from os.path import isfile, join

the_args = sys.argv
path = the_args[1]

#we look for the .l and .y files (commonly lexer.l, parser.y)
for file in listdir(path):
    if file.endswith(".l"):
        #this condition is necessary to avoid hidden files
        if (file[0] != '.'):
            base_lex_file = file
            lex_file = join(path, file)
    if file.endswith(".y"):
        if (file[0] != '.'):
            base_parse_file = file
            parse_file = join(path, file)

# lex_file = path + "/lexer.l"
# parse_file = path + "/parser.y"

#system("rm copied/*")
#onlyfiles = [f for f in listdir(path) if isfile(join(path, f))]

read_defs = open("token_definitions", "r")
defs = read_defs.read()
read_defs.close()

contents = ""
to_read = open(parse_file, "r")

for line in to_read:
    substring = "int main("
    #find all lines who include files
    if substring in line:
        break
    contents += line

to_read.close()


start = ""
end = ""
first_half = True
to_read = open(lex_file, "r")
substring1 = "#include \"parser.hpp\""
substring2 = "#include \"parser.h\""

for line in to_read:
    if (first_half == True):
        start += line
    else:
        end += line
    if ((substring1 in line) or (substring2 in line)):
        first_half = False

to_read.close()

#need basename of user's %.l file (might not be named 'lexer.l') same for parser.y
to_write = open("copied/" + base_lex_file, "w")
to_write.write(start)
to_write.write("\n" + defs)
to_write.write(end)
to_write.close()


example = open("sample", "r")
main_func = example.read()
example.close()

#system("cp " + path + " * copied/")
#system("cd copied/")
#system("rm parser.y")

to_write = open("copied/" + base_parse_file, "w")
to_write.write(contents)
to_write.close()

to_append = open("copied/" + base_parse_file, "a")
to_append.write("\n")
to_append.write(main_func)
to_append.close()

# to_execute = ""
# for h in list_of_headers:
#     to_execute += path + "/" + h + " "
#     system("cp " + path + "/" + h + " .")

# print(to_execute)
# exit(0)

system("make -C copied/")

print("Operation finished!")
#system("cd ~/Documents/Diplomatiki/Lexer")
