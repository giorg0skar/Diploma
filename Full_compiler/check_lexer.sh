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

if [ "$#" -gt 1 ];
then
    executable="$2"
else
    executable="ldanac.sh"
fi

./danac $input
exit_status=$?
if [ $exit_status -eq 1 ];
then
    #correct compiler detected error
    ./$executable $input
    exit_status=$?
    if [ $exit_status -eq 1 ];
    then
        #user's compiler detected errors too. Exit correctly
        echo "Success! Error caught"
        exit 0
    else
        echo "Errors where not detected when some exist"
        exit 1
    fi
else
    #input contained no errors
    ./$executable $input
    exit_status=$?
    if [ $exit_status -eq 1 ];
    then
        echo "Errors were detected when none exist"
        exit 1
    else
        echo "Lexical analysis has no errors"
    fi
fi

# ./ldana < $input > $user_output 2> $errors

#delete all the files produced by the correct compiler
#rm real_errors
#rm output