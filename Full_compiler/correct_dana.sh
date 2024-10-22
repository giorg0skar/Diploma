#!/bin/bash

#takes as input only .dna files
Oflag=0
createir=false
createasm=false
file=""

while [ $# -gt 0 ]
do
    case "$1" in
        -O0) Oflag=0;;
        -O1) Oflag=1;;
        -O2) Oflag=2;;
        -O3) Oflag=3;;
        -f)  createasm=true;;
        -i)  createir=true;;
        -*)  ;;
        *)   file="$1";;
    esac
    shift

done

optflag=""
if [ $Oflag -ne 0 ]; 
then
    optflag=-O$Oflag
fi

if [ $file == "" ];
then
    ./dana > 1.ll || exit 1
    opt $optflag 1.ll -S -o a.ll
    rm 1.ll
    if [ "$createir" = true ] 
    then
        if [ "$createasm" = false ]
        then
            exit 0
        fi
    fi
    llc $optflag a.ll -o a.s
    if [ "$createasm" = true ] 
    then
        exit 0
    fi
    clang a.s ./edsger_lib/lib.a -o a.out
    exit 0
fi

#name=$(echo $file| cut -d'.' -f 1 )
name=${file::-4}
#echo $name
irfile="$name.ll"
asmfile="$name.s"

#compile the input file
#echo "Compiling $file"
./dana < $file > 1.ll || exit 1
#opt $optflag 1.ll -S -o a.ll
opt $optflag 1.ll -S -o "$irfile"
rm 1.ll
if [ "$createir" = true ] 
then
    if [ "$createasm" = false ]
    then
        exit 0
    fi
fi
#llc $optflag a.ll -o a.s
llc $optflag "$irfile" -o "$asmfile"
if [ "$createasm" = true ] 
then
    exit 0
fi
#clang a.s ./edsger_lib/lib.a -o a.out
clang "$asmfile" ./edsger_lib/lib.a -o prog
#rm 1.ll
exit 0