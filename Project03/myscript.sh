#!/bin/bash

if [ ! -d "$2" ]
then
	mkdir "$2"
fi

gcc dimensions.c ImageIO/RasterImage.c ImageIO/imageIO_TGA.c -o dimensions
gcc crop.c ImageIO/RasterImage.c ImageIO/imageIO_TGA.c -o crop 
gcc rotate.c ImageIO/RasterImage.c ImageIO/imageIO_TGA.c -o rotate
gcc split.c ImageIO/RasterImage.c ImageIO/imageIO_TGA.c -o split


for f in "$1"/*
do
	filename=$(basename -- "$f")
	extension=${filename##*.}
	if [ "$extension" == "tga" ]
	then
		# get the width and height of image using dimension program
		width=$(./dimensions -w "$f")
		height=$(./dimensions -h "$f")

		# this to get the center of image, but also
		# represents the width and heigh of each of the four images
		nwidth=$((width/2))
		nheight=$((height/2))

		lisofx=(0 $nwidth)
		listofy=($nheight 0)
		for x in ${lisofx[@]}
		do
			for y in ${listofy[@]}
			do
				./crop $x $y $nwidth $nheight "$f" "$2"
			done
		done	

		./split "$f" "$2"
		./rotate l "$f" "$2"
		
	fi
done
