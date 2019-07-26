#!/bin/bash

#this script will run semantic analysis comparing a student's compiler with a correct compiler
#a random executable is chosen for testing

# path_to_here="/home/george/Documents/Diplomatiki"
# path_to_uc="/home/george/Documents/Diplomatiki/Students_dana_2017/dimitriadis-zamanis-spantouris/src"
path_to_here="$1"
path_to_uc="$2"

make -C Lexer/
make -C $path_to_uc

# if the user has specified the name of his compiler's executable file then we read it.
# otherwise we consider the newest executable after 'make' the compiler exe file
# if [ -s $path_to_uc/COMPILER ]
# then
#     cp $path_to_uc/COMPILER .
# else
#     ./check.sh $path_to_uc
# fi

correct_lex="$path_to_here/Lexer/dana"


if [ -s COMPILER ];
then
    cd $path_to_uc
    $path_to_here/Lexer/lexer_tests.py $path_to_here 1 $correct_lex
    exit_status=$?
    cd $path_to_here
else
    cd $path_to_uc
    $path_to_here/Lexer/lexer_tests.py $path_to_here 0 $correct_lex
    exit_status=$?
    cd $path_to_here
fi

if [ $exit_status -eq 1 ];
then
    exit 1
fi

exit 0