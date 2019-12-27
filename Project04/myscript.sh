#!/bin/bash

# check for correct number of arguments
if [ "$#" -ne 2 ]
then
	echo "Error incorrect number of arguments"
	echo "Proper usage bash myscript.sh [path to directory of image(s)] [path to output directory]"
	exit 2
fi


# check if path to directory to images is valid if not exit the script
if [ ! -d "$1" ]
then
	echo "Path $1 does not exists"
	exit 2
fi

# check if the path to output directory is valid if not create the directory
if [ ! -d "$2" ]
then
	mkdir "$2"
fi

# compile the tile program
gcc tile.c ImageIO/imageIO_TGA.c ImageIO/RasterImage.c -o tile

# create image list
imagelist=()

# add all the tga image from input directory to imagelist
for f in "$1"/*
do
	filename=$(basename -- "$f")
	extension=${filename##*.}
	if [ "$extension"="tga" ]
	then
		imagelist+=("$f")
	fi
done

# sort the input images
sortlist=( $(for item in "${imagelist[@]}"
			do
				echo "$item"
			done | sort) )

# output img name
outputimg=""

# loop 100 times until find the filename that does not exists
for i in {0..100}
do
	outputimg="$2/composite$i.tga"
	if [ ! -f "$outputimg" ]
	then
		break
	fi
done

# run the tile program 
./tile "${sortlist[@]}" "$outputimg"

