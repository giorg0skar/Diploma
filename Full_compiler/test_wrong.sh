#!/bin/bash

if [ "$#" -lt 2 ];
then
    echo "Need to give more arguments"
    exit 1
fi

#system_test Arguments:
# 1. compiler_executable (needs the whole path)
# 2. specific_language_file 

#create an executable from the input written in the specific language we want to test
comp_name=$1
#correct_compiler=$2
input_prog=$2

#eval $comp_name $input_prog
/home/george/Documents/Diplomatiki/Students_dana_2017/dimitriadis-zamanis-spantouris/src/compile.sh -l $input_prog
exit_status=$?

#we check if source program compiles without errors
if [ $exit_status -eq 0 ];
then
    #compiling finished correctly. since we're testing the wrong programs that means that the compiler didn't catch an error
    echo "Compiler didn't catch the error"
    exit 1
fi

echo "Recognised error correctly"

exit 0