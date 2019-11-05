#!/bin/bash

#list=`find Semantic/ -maxdepth 1 -type f`
#echo $list

path="$1"
bit=0

#make -C $path/

for f in $path/* ;
do
    if [ -x $f ];
    then
        #echo $f
        if [ $bit -eq 0 ];
        then
            max=$f
            bit=1
        else
            if [ $f -nt $max ];
            then
                max=$f
            fi
        fi
    fi
done

echo $max
echo $max > EXECUTABLE
echo ""
#make -C $path/ clean

exit 0