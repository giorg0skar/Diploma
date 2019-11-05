#!/usr/bin/python

import sys
import io
import random
from os import listdir, system
from os.path import isfile, join


class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

# system("rm tests/*.out")
# system("rm tests/*.err")

#compiler_to_test = sys.argv[1]
path_to_here = sys.argv[1]

#read the name of the compiler executable from a text file
check = int(sys.argv[2])
if (check == 1):
    file_to_read = open(sys.argv[1] + "/COMPILER", "r")
    compiler_to_test = (file_to_read.readline()).rstrip()
    file_to_read.close()
else:
    #if the user hasn't specified a name for his compiler we assume the executable is named 'compiler'
    compiler_to_test = sys.argv[3] + "/compiler"

#list of all files in tests/ directory (only files)
path_to_suite = sys.argv[1] + "/Testsuite"
path_to_correct = path_to_suite + "/Correct/"
path_to_wrong = path_to_suite + "/Wrong/"

correct_tests = listdir(path_to_correct)
wrong_tests = listdir(path_to_wrong)
#correct_tests = [f for f in listdir(path_to_correct) ]
#wrong_tests = [f for f in listdir(path_to_wrong) ]

#onlyfiles = [f for f in listdir("tests/") if isfile(join("tests/", f))]
#print (onlyfiles)

correct_cases = 0
wrong_cases = 0
count = 1
wrong_list = []


print(bcolors.OKBLUE + "Correct test cases:" + bcolors.ENDC)

for test in correct_tests:
    inputname = path_to_correct + test
    print( str(count) + ".Running correct parser test: " + test)
    print ("--------------------------------")
    # x = system("./check_semantic.sh " + inputname + " " + compiler_to_test + " " + correct_sem)
    x = system(compiler_to_test + " < " + inputname)

    val = x >> 8
    if (val == 1):
        print(bcolors.WARNING + "Errors were detected when none exist" + bcolors.ENDC)
        wrong_cases += 1
        wrong_list.append(count)
    else:
        print(bcolors.OKGREEN + "No errors so syntax analysis finishes" + bcolors.ENDC)
        correct_cases += 1
    print("")
    count += 1


print("")
print(bcolors.OKBLUE + "Purposefully wrong test cases:" + bcolors.ENDC)

for test in wrong_tests:
    inputname = path_to_wrong + test
    print( str(count) + ".Running wrong parser test: " + test)
    print ("-------------------------------")
    # x = system("./check_semantic.sh " + inputname + " " + compiler_to_test + " " + correct_sem)
    x = system(compiler_to_test + " < " + inputname)

    val = x >> 8
    if (val == 1):
        print(bcolors.OKGREEN + "Error caught" + bcolors.ENDC)
        correct_cases += 1
    else:
        print(bcolors.WARNING + "Errors not detected when some exist" + bcolors.ENDC)
        wrong_cases += 1
        wrong_list.append(count)
    print("")
    count += 1

total = correct_cases + wrong_cases
#total = len(correct_tests) + len(wrong_tests)
print("STATISTICS\n")

print("Correct cases: 1 - " + str(len(correct_tests)))
low = len(correct_tests) + 1
print("Cases with errors: " + str(low) + " - " + str(low-1 + len(wrong_tests)) + "\n")

print( bcolors.OKGREEN + "Number of correct test cases: " + str(correct_cases) + "/" + str(total) + bcolors.ENDC )
if (wrong_cases > 0):
    print( bcolors.FAIL + "Number of failed test cases: " + str(wrong_cases) + "/" + str(total) + " : " + str(wrong_list) + bcolors.ENDC )
    exit(1)
else:
    print(bcolors.OKGREEN + "Compiler is syntactically correct!\n" + bcolors.ENDC)
    exit(0)

# system("rm " + path_to_correct + "*.err")
# system("rm " + path_to_correct + "*.out")
# system("rm " + path_to_wrong + "*.err")
# system("rm " + path_to_wrong + "*.out")