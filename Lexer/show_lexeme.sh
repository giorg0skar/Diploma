#!/bin/bash

input="$1"
name=${input::-4}
newfile="$name.lex"
rm $newfile

./dana < $input > output 2> errors
cat errors
python lexMap.py output errors > $newfile
rm output
rm errors
cat $newfile