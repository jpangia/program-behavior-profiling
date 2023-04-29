#!/bin/bash

# check for passed executable
# currentDir=$(pwd)
if ! test -x "$1"
then
    echo "executable \`$1\` not found"
    echo "usage: ins-count <relative path to executable> [arguments to executable]"
    exit
fi

# create the csv if it does not exist
csv=data.csv
if ! test -f $csv
then
    echo "created $csv"
    echo "program,number of instructions" > $csv
fi

# create the temp folder if it does not exist
tmpFolder=/tmp/ins-count
if ! test -d $tmpFolder
then
    echo "created $tmpFolder"
    mkdir $tmpFolder
fi

# execute the program, writing the instruction count to the csv
valgrind --log-file=/tmp/ins-count/valgrind.tmp --tool=cachegrind --cachegrind-out-file=/dev/null ./${@}
numIns=$(grep -E "I\s*refs:\s*[\d,]*" /tmp/ins-count/valgrind.tmp | grep -P -o "[\d,]*$")
echo "$1,\"$numIns\"" >> $csv