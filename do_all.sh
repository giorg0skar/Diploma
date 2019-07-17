#!/bin/bash

# run every step of the compiler check

# path_to_here: path to the current directory
# path_to_uc: path to the directory that contains the source files of the compiler to-be-tested (uc: abbreviation of user's compiler)

path_to_here="/home/george/Documents/Diplomatiki"
path_to_uc="$path_to_here/Students_dana_2017/dimitriadis-zamanis-spantouris/src"

make -C $path_to_uc

if [ -s $path_to_uc/COMPILER ]
then
    # cp $path_to_uc/COMPILER .
    text=`cat $path_to_uc/COMPILER`
    echo "$path_to_uc/$text" > COMPILER
# else
#     ./check.sh $path_to_uc
fi

./run_lexer $path_to_here $path_to_uc
exitl=$?
./run_parser $path_to_here $path_to_uc
exitp=$?
./run_semantic $path_to_here $path_to_uc
exits=$?
./run_full_compiler $path_to_here $path_to_uc
exitf=$?

echo ""
echo "-----------------"
if [ $exitl -eq 0 ];
then
    echo "LEXER: passed"
else
    echo "LEXER: failed"
fi

if [ $exitp -eq 0 ];
then
    echo "PARSER: passed"
else
    echo "PARSER: failed"
fi

if [ $exits -eq 0 ];
then
    echo "SEMANTIC: passed"
else
    echo "SEMANTIC: failed"
fi

if [ $exitf -eq 0 ];
then
    echo "EXECUTION: passed"
else
    echo "EXECUTION: failed"
fi

echo "-----------------"

exit 0