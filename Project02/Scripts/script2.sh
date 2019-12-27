#!/bin/bash

# change directory to the home directory
cd $HOME

# represents the file ext 
extlist=(cpp c hpp h sh txt)
# represents array of the ordered file
VALUE=()
# represents array of all of the file basename with ext
FILELIST=()

# checks if directory
if [[ ! -d $1 ]]
then 
	echo "Directory does not exists"  	
else
	# all /* so we can see all files withint the directory
	FILES="${1}/*"
	# traverse through all the file adding all files to FILELIST
	for f in $FILES
	do 
		filename=$(basename $f)
		if [[ -f $f ]]
		then
			FILELIST+=($filename)
		fi
	done

	# traverse through list of extensions and adding all the files in the right order
	# removing the file from FILELIST at the same time
	for ext in ${extlist[@]}
	do
		for f in $FILES
		do
			filename=$(basename $f)
			#checks if is file and contains the correct extension
			if [[ -f $f && $ext = ${filename##*.} ]]
			then
				# remove file from the filelist
				FILELIST=("${FILELIST[@]/$filename}")
				# add file to VALUE
				VALUE+=($filename)
			fi
		done	
	done

	VALUE+=(${FILELIST[@]})
	
	for val in ${VALUE[@]}
	do
		echo $val
	done
fi
