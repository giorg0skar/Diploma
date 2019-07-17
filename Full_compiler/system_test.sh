#!/bin/bash

if [ "$#" -lt 3 ];
then
    echo "Need to give more arguments"
    exit 1
fi

#system_test Arguments:
# 1. compiler_executable (needs the whole path)
# 2. correct compiler
# 3. specific_language_file 
# 4. expected output file
# 5. [ input_to_executable ]

#create an executable from the input written in the specific language we want to test
comp_name=$1
correct_compiler=$2
input_prog=$3

$comp_name $input_prog
exit_status=$?

#we check if source program compiles without errors
if [ exit_status -ne 0];
then
    #error in compiling. need to check with correct compiler
    $correct_compiler $input_prog
    exit_status=$?
    if [ exit_status -ne 0 ];
    then
        echo "Error caught in compiling $input_prog"
        exit 0
    else
        echo "Compile error in $input_prog"
        exit 1
    fi
fi

expected_output=$4

if [ "$#" -gt 4 ];
then
    input=$5
    ./a.out < $input > user_output
else
    ./a.out > user_output
fi

#we check if the output of the executable we created is the same as the correct one
diff -s user_output $expected_output
exit_status=$?

if [ exit_status -ne 0 ];
then
    echo "Error, output mismatch in $lang_input"
    exit 1
fi

exit 0