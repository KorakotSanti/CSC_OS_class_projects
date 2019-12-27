#!/bin/bash


# change directory to the Home Directory first
cd $HOME

# this checks if argument the user input is a directory or not 
if [[ -d $1 ]]
then 
	# append a /* at the end of directory to get the list of files and folders in that directory
	FILES="${1}/*"

	# traverse the list or files and folders
	for f in $FILES
	do

	# check if the item in directory is a folder/dirctory or a file
	# if it is a file proceed
	if [[  -f $f ]]
	then
	# this prints out the basename of the file and not the whol directory
  	echo $(basename $f)
	fi
	done
else
	echo "Directory does not exists"
fi

