#!/usr/bin/python

import sys
import io
import random
from os import listdir, system
from os.path import isfile, join, basename

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'


#list of all files in tests/ directory (only files)
# onlyfiles = [f for f in listdir("tests/") if isfile(join("tests/", f))]
#   ARGUMENTS:
# 1. path to the project directory
# 2. flag to indicate if there's a specific name for the compiler
# 3. name of the correct compiler (used for comparison)

check = int(sys.argv[2])
if (check == 1):
    file_to_read = open(sys.argv[1] + "/COMPILER", "r")
    compiler_to_test = (file_to_read.readline()).rstrip()
    file_to_read.close()
else:
    #if the user hasn't specified a name for his compiler we assume the executable is named 'compiler'
    compiler_to_test = "./compiler"

correct_lex = sys.argv[3]

#list of all files in tests/ directory (only files)
path_to_suite = sys.argv[1] + "/Testsuite"
path_to_correct = path_to_suite + "/Correct/"
path_to_wrong = path_to_suite + "/Wrong/"

correct_tests = listdir(path_to_correct)
wrong_tests = listdir(path_to_wrong)

correct_cases = 0
wrong_cases = 0
count = 1
wrong_list = []
base = basename(compiler_to_test)

for test in correct_tests:
    inputname = path_to_correct + test
    print ( str(count) + ". Running test: " + test)
    print ("----------------------")
    # x = system("./check_lexer.sh " + inputname + " " + compiler_to_test)
    x = system(compiler_to_test + " < " + inputname + " > user_output")
    y = system(correct_lex + " < " + inputname + " > output")

    val = x >> 8
    if (val == 1):
        wrong_cases += 1
        wrong_list.append(count)
    else:
        x = system("diff -s user_output output")
        system("rm output user_output")
        val = x >> 8
        if (val == 1):
            print(bcolors.BOLD + "Error, output mismatch in " + test + bcolors.ENDC)
            wrong_cases += 1
            wrong_list.append(count)
        else:
            correct_cases += 1
    print ("")
    count += 1

for test in wrong_tests:
    inputname = path_to_wrong + test
    print ( str(count) + ". Running test: " + test)
    print ("----------------------")
    # x = system("./check_lexer " + inputname + compiler_to_test + correct_lex)
    x = system(compiler_to_test + " < " + inputname + " > user_output")
    y = system(correct_lex + " < " + inputname + " > output")

    val = x >> 8
    correct_val = y >> 8
    if (val == 1):
        if (correct_val == 0):
            wrong_cases += 1
            wrong_list.append(count)
        else:
            print("Error caught correctly")
            correct_cases += 1
    else:
        x = system("diff -s user_output output")
        system("rm output user_output")
        val = x >> 8
        if (val == 1):
            print(bcolors.BOLD + "Error, output mismatch in " + test + bcolors.ENDC)
            wrong_cases += 1
            wrong_list.append(count)
        else:
            correct_cases += 1
    count += 1
    print("")

total = correct_cases + wrong_cases
print("STATISTICS\n")

print("Correct cases: 1 - " + str(len(correct_tests)))
low = len(correct_tests) + 1
print("Cases with errors: " + str(low) + " - " + str(low-1 + len(wrong_tests)) + "\n")

print(bcolors.OKGREEN + "number of correct test cases: " + str(correct_cases) + "/" + str(total) + bcolors.ENDC)
if (wrong_cases > 0):
    print(bcolors.FAIL + "number of wrong test cases: " + str(wrong_cases) + "/" + str(total) + " : " + str(wrong_list) + bcolors.ENDC)