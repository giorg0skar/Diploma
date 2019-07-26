#!/bin/bash

#this script will run semantic analysis comparing a student's compiler with a correct compiler
#a random executable is chosen for testing

# path_to_here="/home/george/Documents/Diplomatiki/Diploma"

path_to_here="$1"
path_to_uc="$2"

make -C $path_to_here/Semantic/
make -C $path_to_uc

# if the user has specified the name of his compiler's executable file then we read it.
# otherwise we consider the newest executable after 'make' the compiler exe file

if [ -s $path_to_uc/COMPILER ]
then
    text=`cat $path_to_uc/COMPILER`
    echo "$path_to_uc/$text" > COMPILER
fi

correct_sem="$path_to_here/Semantic/dana"


if [ -s COMPILER ];
then
    # cd $path_to_uc
    $path_to_here/Semantic/semantic_tests.py $path_to_here 1 $path_to_uc
    exit_status=$?
    # cd $path_to_here
else
    $path_to_here/Semantic/semantic_tests.py $path_to_here 0 $path_to_uc
    exit_status=$?
fi

if [ $exit_status -eq 1 ];
then
    exit 1
fi

exit 0