#!/usr/bin/python

import sys
import io
import random
from os import listdir, system, access, X_OK
from os.path import isfile, join, basename

def is_exe(file):
    return isfile(file) and access(file, X_OK)

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'


the_args = sys.argv

#compiler_to_test = sys.argv[1]
path_to_here = sys.argv[1] + "/Full_compiler"
correct_sem = sys.argv[2]

#list of all files in tests/ directory (only files)
path_to_suite = sys.argv[1] + "/Testsuite"
path_to_correct = path_to_suite + "/Correct/"
path_to_wrong = path_to_suite + "/Wrong/"
path_to_output = path_to_suite + "/Correct_Output/"
path_to_input = path_to_suite + "/Input"
path_to_tested = sys.argv[4]

correct_tests = listdir(path_to_correct)
wrong_tests = listdir(path_to_wrong)

# print (correct_tests)
# print ("")
# print(wrong_tests)

# exit(0)


#read the name of the compiler executable from a text file
check = int(sys.argv[3])
if (check == 1):
    file_to_read = open(sys.argv[1] + "/EXECUTABLE", "r")
    compiler_to_test = (file_to_read.readline()).rstrip()
    #print (compiler_to_test)
    file_to_read.close()
    #print (compiler_to_test)
else:
    #if the user hasn't specified a name for his compiler we assume the executable is named 'compiler'
    compiler_to_test = "./compiler"

correct_cases = 0
wrong_cases = 0
count = 1
wrong_list = []


print(bcolors.OKBLUE + "Correct test cases:" + bcolors.ENDC)

for test in correct_tests:
    inputname = path_to_correct + test
    expected_output = path_to_output + test[:-4] + ".out"
    print( str(count) + ". Running correct test: " + test)
    print ("--------------------------------")
    # to_execute = path_to_here + "/test_correct.sh " + compiler_to_test + " "  + inputname + " " + expected_output + " " + path_to_input
    # x = system(to_execute )
    x = system(compiler_to_test + " " + inputname)
    val = x >> 8
    if (val == 1):
        print(bcolors.BOLD + "Error in compiling\n" + bcolors.ENDC)
        wrong_cases += 1
        wrong_list.append(count)
        count += 1
        continue
    
    print("Compiling finished")
    inname = path_to_input + "/" + test[:-4] + ".in"
    if (isfile(inname)):
        system(path_to_correct + "a.out < " + inname + " > user_output")
    else:
        system(path_to_correct + "a.out > user_output")
    
    x = system("diff -s user_output " + expected_output)
    system("rm user_output")
    #print("")
    val = x >> 8
    if (val == 1):
        print(bcolors.BOLD + "Error, output mismatch in " + test + bcolors.ENDC)
        wrong_cases += 1
        wrong_list.append(count)
    else:
        correct_cases += 1
    count += 1
    #we delete the a.out executable produced from the compiler
    #system("rm " + path_to_tested + "/a.out")
    system("rm " + path_to_correct + "a.out 2> /dev/null")
    print("")

print("")
print(bcolors.OKBLUE + "Purposefully wrong test cases:" + bcolors.ENDC)

for test in wrong_tests:
    inputname = path_to_wrong + test
    print( str(count) + ". Running wrong test: " + test)
    print ("-------------------------------")

    #print(bcolors.WARNING)
    # to_execute = path_to_here + "/test_wrong.sh " + compiler_to_test + " " + inputname
    # x = system( to_execute )
    x = system(compiler_to_test +  " " + inputname)
    #print(bcolors.ENDC)

    val = x >> 8
    if (val == 1):
        print("Recognised error correctly")
        correct_cases += 1
    else:
        print("Compiler didn't catch the error")
        wrong_cases += 1
        wrong_list.append(count)
    print("")
    count += 1

total = correct_cases + wrong_cases
print("STATISTICS\n")

print("Correct cases: 1 - " + str(len(correct_tests)))
low = len(correct_tests) + 1
print("Cases with errors: " + str(low) + " - " + str(low-1 + len(wrong_tests)) + "\n")

print( bcolors.OKGREEN + "number of correct test cases: " + str(correct_cases) + "/" + str(total) + bcolors.ENDC )
if (wrong_cases > 0):
    print( bcolors.FAIL + "number of failed test cases: " + str(wrong_cases) + "/" + str(total) + " : " + str(wrong_list) + bcolors.ENDC )
else:
    print(bcolors.OKGREEN + "Compiler is running correctly!\n" + bcolors.ENDC)


# cleanup
system("rm " + path_to_correct + "*.imm")
system("rm " + path_to_correct + "*.asm")
system("rm " + path_to_wrong + "*.imm")
system("rm " + path_to_wrong + "*.asm")
#system("rm " + path_to_correct + "a.out")
system("rm " + path_to_wrong + "a.out")
system("rm " + path_to_wrong + "a.ll")

if (wrong_cases > 0):
    exit(1)
else:
    exit(0)