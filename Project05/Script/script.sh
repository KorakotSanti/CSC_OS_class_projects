#!/bin/bash

# check number of arguments
if [ $# -ne 2 ]
then
    echo "Invalid arguments"
    exit 1
fi

input=$1
output=$2
newinput=""
# check if the data directory is valid
if [ ! -d $input ]
then
    echo "The directory $input does not exists"
    exit 1
fi

# check if data directory is empty
if [  ! "$(ls -A $input)" ]
then
    echo "Data Directory is empty"
    exit 1
fi

#string len of input dirctory
strlen=$((${#input}-1))

#check if ending of input directory path ends with / going to remove it
# by redefining input leaving out the last character
if [ ${input:$strlen:1} = "/" ]
then
    input=${input::-1}
fi

# extra credit, make a new directory same name as input directory append with _fixed
# this will check if the new directory exists first if it doesn't make the new directory
# otherwise will put an loop adding a new index after _fixed until found a new directory
# that does not exists
if [ ! -d "${input}_fixed" ]
then
    newinput="${input}_fixed/"
    mkdir $newinput
else
    index=0
    while true
    do
        if [ ! -d "${input}_fixed${index}" ]
        then
            newinput="${input}_fixed${index}/"
            mkdir $newinput
            break
        fi
        index=$((index+1))
    done
fi

# n represents number of process that needs to create in the future program that will be runned
n=0
# traverse through directory of files
for dir in $input/*
do
    # get filename
    filename=$(basename "$dir")
    # get fileline from file with the visualizer
    # -v option visualizes CRLF format '\r' or '\r\n' as ^M
    fileline=$(cat -v "$dir")

    # gets the first digits which is the index in the file
    result=$(grep -Eo "^[[:digit:]]*[[:digit:]]" "$dir")

    # get the highest index in list of files
    if [ "$n" -lt "$result" ]
    then
        n=$result
    fi

    #check if the text file ends with ^M
    if [ "${fileline: -2}" = "^M" ]
    then
        echo "${fileline::-2}" > $newinput$filename
    else
        echo "${fileline}" > $newinput$filename
    fi
done

n=$((n+1))
echo $n

g++ ../Source/Version5.cpp -o Version5
./Version5 $n $newinput $output