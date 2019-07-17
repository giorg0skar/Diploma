#!/bin/bash

#execute full compiler tests

#this script will run semantic analysis comparing a student's compiler with a correct compiler
#a random executable is chosen for testing

path_to_here="$1"
path_to_uc="$2"
# path_to_here="/home/george/Documents/Diplomatiki"
# path_to_uc="/home/george/Documents/Diplomatiki/Students_dana_2017/dimitriadis-zamanis-spantouris/src"

#make -C Full_compiler/
make -C $path_to_uc

correct_sem="$path_to_here/Full_compiler/danac"

# if the user has specified the name of his compiler's executable file then we read it.
# otherwise we consider the newest executable after 'make' the compiler exe file
if [ -s $path_to_uc/EXECUTABLE ]
then
    cp $path_to_uc/EXECUTABLE .
fi

# echo "/home/george/Documents/Diplomatiki/Students_dana_2017/dimitriadis-zamanis-spantouris/src/compile.sh -l" | tee EXECUTABLE
#compiler_tests.py path_to_current_dir name_of_correct_compiler is_there_name_of_uc

if [ -s EXECUTABLE ]
then
    cd $path_to_uc
    $path_to_here/Full_compiler/compiler_tests.py $path_to_here $correct_sem 1 $path_to_uc
    exit_status=$?
    cd $path_to_here
else
    cd $path_to_uc
    $path_to_here/Full_compiler/compiler_tests.py $path_to_here $correct_sem 0 $path_to_uc
    exit_status=$?
    cd $path_to_here
fi

#rm EXECUTABLE

if [ $exit_status -eq 1 ];
then
    exit 1
fi

exit 0