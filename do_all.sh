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

# echo "Give the path to the project directory "
# read -r path_to_here
# echo "Give the path to the compiler under test home directory"
# read -r path_to_uc

path_to_here="/home/george/Documents/Diplomatiki/Diploma"
path_to_uc="/home/george/Documents/Compilers"

echo "You may be asked to give some directories for each compiler phase below."
echo "Since the home dir is given only the directory name for each phase need to be given i.e Lexer. Full path is not required"

if [ "$runlexer" = true ];
then
    echo "Give Lexer directory"
    # read -r lexer_dir
    # $path_to_here/run_lexer.sh $path_to_here "$path_to_uc/$lexer_dir"
    $path_to_here/run_lexer.sh $path_to_here "$path_to_uc/Lexer"
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
    echo "Give Parser directory"
    # read -r parser_dir
    # $path_to_here/run_parser.sh $path_to_here "$path_to_uc/$parser_dir"
    $path_to_here/run_parser.sh $path_to_here "$path_to_uc/Parser"
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
    echo "Give Semantic analysis directory"
    # read -r sem_dir
    # $path_to_here/run_semantic.sh $path_to_here "$path_to_uc/$sem_dir"
    $path_to_here/run_semantic.sh $path_to_here "$path_to_uc/Sem_analysis"
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
    echo "Give full compiler directory"
    # read -r compiler_dir
    # if [ $compiler_dir = "" ];
    # then
    #     $path_to_here/run_full_compiler.sh $path_to_here "$path_to_uc"
    # else
    #     $path_to_here/run_full_compiler.sh $path_to_here "$path_to_uc/$compiler_dir"
    # fi
    $path_to_here/run_full_compiler.sh $path_to_here "$path_to_uc/LLVM"
    exitf=$?
    if [ $exitf -eq 0 ];
    then
        echo "EXECUTION: passed"
    else
        echo "EXECUTION: failed"
    fi
    exit 0
fi

# these paths need to be fixed to run each compiler phase
echo "Give Lexer directory"
# read -r lexer_dir
# $path_to_here/run_lexer.sh $path_to_here "$path_to_uc/$lexer_dir"
$path_to_here/run_lexer.sh $path_to_here "$path_to_uc/Lexer"
exitl=$?

echo "Give Parser directory"
# read -r parser_dir
# $path_to_here/run_parser.sh $path_to_here "$path_to_uc/$parser_dir"
$path_to_here/run_parser.sh $path_to_here "$path_to_uc/Parser"
exitp=$?

echo "Give Semantic analysis directory"
# read -r sem_dir
# $path_to_here/run_semantic.sh $path_to_here "$path_to_uc/$sem_dir"
$path_to_here/run_semantic.sh $path_to_here "$path_to_uc/Sem_analysis"
exits=$?

echo "Give full compiler directory"
# read -r compiler_dir
# $path_to_here/run_full_compiler.sh $path_to_here "$path_to_uc/$compiler_dir"
$path_to_here/run_full_compiler.sh $path_to_here "$path_to_uc/LLVM"
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