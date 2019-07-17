#!/bin/bash

if [ "$#" -lt 3 ];
then
    echo "Need to give more arguments"
    exit 1
fi

#system_test Arguments:
# 1. compiler_executable 
# 2. specific_language_file 
# 3. executable_name 
# 4. [ input_to_executable ]

#create an executable from the input written in the specific language we want to test
comp_name="$1"
lang_input="$2"
./$comp_name $lang_input
exit_status=$?
# if [ $exit_status -ne 0 ];
# then
#     echo "Compile error in $lang_input"
#     exit 1
# fi

if [ $exit_status -ne 0 ];
then
    #needs comparison with correct compiler
    #if the target language program has errors the user's compiler must detect them in order to display correct behavior
    ./ldanac.sh $lang_input
    exit_status=$?
    if [ $exit_status -ne 0 ];
    then
        echo "Error caught in $lang_input"
        exit 0
    else
        echo "Compile error in $lang_input"
        exit 1
    fi
else
    #program compiled successfully
    ./ldanac.sh $lang_input
    exit_status=$?
    if [ $exit_status -ne 0 ];
    then
        echo "Errors not detected in $lang_input"
        exit 1
    fi
fi

#correct_exec=${lang_input::-4}
correct_exec="$3"

if [ "$#" -gt 3 ];
then
    prog_input="$4"
    ./a.out < $prog_input > user_output
    ./$correct_exec < $prog_input > correct_output
else
    ./a.out > user_output
    ./$correct_exec > correct_output
fi

#rm "$correct_exec.ll"
#rm "$correct_exec.s"

#we check if the output of the executable we created is the same as the correct one
diff -s user_output correct_output
exit_status=$?
if [ $exit_status -ne 0 ];
then
    echo "Output mismatch in $lang_input"
    exit 1
fi
rm user_output
rm correct_output