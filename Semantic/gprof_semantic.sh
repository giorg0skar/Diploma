#!/bin/bash

if [ "$#" -lt 1 ];
then
    echo "Need to give more arguments"
    exit 0
fi

#if it exists, the 2nd argument contains the correct output from $1 program
input="$1"
if [ "$#" -lt 2 ];
then
    ./dana < $input > output 2> real_errors
    if [ -s real_errors ]
    then
        gprof -b -l -qyyerror dana gmon.out > correct_yyerror_analysis
        gprof -b -l -qerror dana gmon.out > correct_error_analysis
    fi
    #rm real_errors
else
    cat "$2" > output
fi

user_output="${input::-4}.out"
errors="${input::-4}.err"
rm $user_output
rm $errors

#python count_lines.py $input
#LD_PRELOAD=./myerror.so ./ldana < $input > $user_output 2> $errors
./ldana < $input > $user_output 2> $errors
if [ -s $errors ]
then
    if [ -s real_errors ]
    then
        cat $errors
        rm yyerror_analysis
        rm error_analysis
        gprof -b -l -qerror ldana gmon.out > error_analysis
        gprof -b -l -qyyerror ldana gmon.out > yyerror_analysis
        python get_stats.py correct_error_analysis correct_yyerror_analysis > cerr
        python get_stats.py error_analysis yyerror_analysis > uerr
        #compare the 2 error files
        echo "uerr: errors found by user, cerr: correct errors"
        diff -s uerr cerr
        
        cat temp_file >> uerr
        echo -e '\n'
        rm temp_file
        rm uerr
        rm cerr
    else
        echo "Program found errors when none exist:"
        echo "-------------------------------------"
        cat $errors
        rm error_analysis
        rm yyerror_analysis
        gprof -b -l -qerror ldana gmon.out > error_analysis
        gprof -b -l -qyyerror ldana gmon.out > yyerror_analysis
        python get_stats.py error_analysis yyerror_analysis > uerr
        cat temp_file >> uerr
        cat uerr
        echo -e '\n'

        rm uerr
        rm temp_file
    fi
else
    if [ -s real_errors ]
    then
        echo "Program didn't catch some errors:"
        echo "---------------------------------"
        cat real_errors
    fi
    diff -s output $user_output
    rm $errors
fi
#delete all the files produced by the correct compiler
rm correct_error_analysis
rm correct_yyerror_analysis
rm real_errors
rm output