#!/bin/bash

#execute full compiler tests

#this script will run semantic analysis comparing a student's compiler with a correct compiler
#a random executable is chosen for testing

path_to_here="$1"
path_to_uc="$2"
# path_to_here="/home/george/Documents/Diplomatiki/Diploma"

make -C $path_to_here/Full_compiler/
make -C $path_to_uc

correct_comp="$path_to_here/Full_compiler/danac"

# if the user has specified the name of his compiler's executable file then we read it.
# otherwise we consider the newest executable after 'make' the compiler exe file
if [ -s $path_to_uc/EXECUTABLE ];
then
    # cp $path_to_uc/EXECUTABLE .
    text=`cat $path_to_uc/EXECUTABLE`
    # echo "$path_to_uc/$text" > EXECUTABLE
    echo "$text" > EXECUTABLE
fi


if [ -s EXECUTABLE ];
then
    $path_to_here/Full_compiler/compiler_tests.py $path_to_here $correct_comp 1 $path_to_uc
    exit_status=$?
else
    $path_to_here/Full_compiler/compiler_tests.py $path_to_here $correct_comp 0 $path_to_uc
    exit_status=$?
fi

#rm 1.ll

if [ $exit_status -eq 1 ];
then
    exit 1
fi

exit 0