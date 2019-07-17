#!/bin/bash

function die () {
  printf "Error in executable\n"
  exit 1
}

if [ "$#" -lt 3 ];
then
    echo "Need to give more arguments"
    exit 1
fi

#system_test Arguments:
# 1. compiler_executable (needs the whole path)
# 2. specific_language_file 
# 3. expected output file
# 4. path to input (.in)

#create an executable from the input written in the specific language we want to test
comp_name=$1
#correct_compiler=$2
input_prog=$2

if [ -z $input_prog ];
then
    echo "No argument for some reason"
    exit 1
fi

# var="$comp_name -l $input_prog"
# eval $var
/home/george/Documents/Diplomatiki/Students_dana_2017/dimitriadis-zamanis-spantouris/src/compile.sh -l $input_prog
exit_status=$?

#we check if source program compiles without errors
if [ $exit_status -ne 0 ];
then
    #error in compiling. since we're testing the correct programs that means that the compiler has an error
    echo "Error in compiling"
    exit 1
fi

echo "Compiling finished"

expected_output=$3
path=$4

#the name of output file is the same as the target language's one , just with .out instead of the specific language suffix 
name=$(basename "$expected_output" .out)
input="$path"/"$name.in"

#if there is an input file then we use it in place of standard input
if [ -s $input ];
then
    /home/george/Documents/Diplomatiki/Testsuite/Correct/a.out < $input > user_output
else
    /home/george/Documents/Diplomatiki/Testsuite/Correct/a.out > user_output
fi

#we check if the output of the executable we created is the same as the correct one
diff -s user_output $expected_output
exit_status=$?
rm user_output

if [ $exit_status -ne 0 ];
then
    echo "Error, output mismatch in $input_prog"
    exit 1
fi

exit 0