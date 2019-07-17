#!/bin/bash

if [ "$#" -lt 1 ];
then
    echo "Need to give more arguments"
    exit 1
fi

#if it exists, the 2nd argument contains the correct output from $1 program
input="$1"
user_output="${input::-4}.out"
errors="${input::-4}.err"
#rm $user_output
#rm $errors

if [ "$#" -gt 2 ];
then
    compiler_to_test="$2"
    correct_compiler="$3"
else
    compiler_to_test="ldana"
    correct_compiler="dana"
fi

$compiler_to_test < $input > $user_output
exit_status=$?
if [ $exit_status -eq 1 ];
then
    #correct compiler detected error
    $correct_compiler < $input > output 2> real_errors
    exit_status=$?
    if [ $exit_status -eq 1 ];
    then
        #user's compiler detected errors too. Exit correctly
        echo "Success! Error caught"
        exit 0
    else
        echo "Errors were detected when none exist"
        exit 1
    fi
else
    #input contained no errors
    $correct_compiler < $input > output 2> real_errors
    exit_status=$?
    if [ $exit_status -eq 1 ];
    then
        echo "Errors were not detected when some exist"
        cat real_errors
        exit 1
    else
        #no errors exist and user's compiler finished successfully
        echo "There are no errors so sem analysis finishes"
        
        exit 0
        
        #diff is redundant in semantic analysis. no output is expected. if no errors are reported the program is expected to be semantically correct
        
    fi
fi

#delete all the files produced by the correct compiler
rm real_errors
rm output