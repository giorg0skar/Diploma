#!/bin/bash

# run every step of the compiler check

# some flags that are used when we want to test a specific phase of the compiler
runlexer=false
runparser=false
runsemantic=false
runfull=false

while [ $# -gt 0 ]
do
    case "$1" in
        -l) runlexer=true;;
        -p) runparser=true;;
        -s) runsemantic=true;;
        -full) runfull=true;;
        -c) runfull=true;;
        -*) ;;
    esac
    shift
done

# path_to_here: path to the current directory
# path_to_uc: path to the directory that contains the source files of the compiler to-be-tested (uc: abbreviation of user's compiler)
# to run every step of the compiler separate directories need to be specified
# the script contains an example of the names those directories may have

# echo "Give path to current directory "
# read -r path_to_here
# echo "Give path to the compiler under test "
# read -r path_to_uc

path_to_here="/home/george/Documents/Diplomatiki/Diploma"
path_to_uc="/home/george/Documents/Compilers"



if [ "$runlexer" = true ];
then
    ./run_lexer.sh $path_to_here "$path_to_uc/Lexer"
    exitl=$?
    if [ $exitl -eq 0 ];
    then
        echo "LEXER: passed"
    else
        echo "LEXER: failed"
    fi
    exit 0
fi

if [ "$runparser" = true ];
then
    ./run_parser.sh $path_to_here "$path_to_uc/Parser"
    exitp=$?
    if [ $exitp -eq 0 ];
    then
        echo "PARSER: passed"
    else
        echo "PARSER: failed"
    fi
    exit 0
fi

if [ "$runsemantic" = true ];
then
    ./run_semantic.sh $path_to_here "$path_to_uc/Sem_analysis"
    exits=$?
    if [ $exits -eq 0 ];
    then
        echo "SEMANTIC: passed"
    else
        echo "SEMANTIC: failed"
    fi
    exit 0
fi

if [ "$runfull" = true ];
then
    ./run_full_compiler.sh $path_to_here "$path_to_uc/LLVM"
    exitf=$?
    if [ $exitf -eq 0 ];
    then
        echo "EXECUTION: passed"
    else
        echo "EXECUTION: failed"
    fi
    exit 0
fi

./run_lexer.sh $path_to_here "$path_to_uc/Lexer"
exitl=$?

./run_parser.sh $path_to_here "$path_to_uc/Parser"
exitp=$?

./run_semantic.sh $path_to_here "$path_to_uc/Sem_analysis"
exits=$?

./run_full_compiler.sh $path_to_here "$path_to_uc/LLVM"
exitf=$?


echo ""
echo "RESULTS"
echo "---------------------------------"
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

echo "---------------------------------"

#cleanup
# make -C Lexer/ distclean
# make -C Parser/ distclean
# make -C Semantic/ distclean
# make -C Full_compiler/ distclean

exit 0