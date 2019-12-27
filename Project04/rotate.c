#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ImageIO/imageIO_TGA.h"

RasterImage rotateLeft(RasterImage* rasimg);
RasterImage rotateRight(RasterImage* rasimg);

// this function returns a new raster of the rotated image to the left
RasterImage rotateLeft(RasterImage* rasimg) {

	//allocate for new Raster
	RasterImage newimg = newRasterImage(rasimg->numCols,rasimg->numRows,RGBA32_RASTER);

	unsigned int row = rasimg->numRows;
	unsigned int col = rasimg->numCols;
	unsigned char* newras=(unsigned char*) newimg.raster;
	unsigned char* inras = (unsigned char*) rasimg->raster;

	// tranverse through all the of the raster and the pixels that stores the color channels
	for(unsigned int i=0;i<row;i++){
		for(unsigned int j=0;j<col;j++){
			for(unsigned int os=0;os<newimg.bytesPerPixel;os++){
				newras[newimg.bytesPerPixel*(j*row+row-i-1)+os] = inras[newimg.bytesPerPixel*(i*col+j)+os];
			}
		}
	}
	return newimg;
}

//same as rotate Left function except it rotates right
// all the logic is same except for storing the values
RasterImage rotateRight(RasterImage* rasimg) {
	RasterImage newimg = newRasterImage(rasimg->numCols,rasimg->numRows,RGBA32_RASTER);

	unsigned int row = rasimg->numRows;
	unsigned int col = rasimg->numCols;
	unsigned char* newras=(unsigned char*) newimg.raster;
	unsigned char* inras = (unsigned char*) rasimg->raster;

	for(unsigned int i=0;i<row;i++){
		for(unsigned int j=0;j<col;j++){
			for(unsigned int os=0;os<newimg.bytesPerPixel;os++){
				newras[newimg.bytesPerPixel*(row*(col-1-j)+i)+os] = inras[newimg.bytesPerPixel*(i*col+j)+os];
			}
		}
	}
	return newimg;
}

int main(int argc, char** argv){

	RasterImage rasimg;
    
	// color to represent the colors
	char *word=" [rotated]";
	char *ext=".tga";
	char *outfile;
	unsigned char *rotatedRas;


	if (argc!=4) {
		printf("Not enough arguments or too much arguments\n");
	}
	else {

		char* specs = argv[1];
		char* infile =argv[2];
		char* directory = argv[3];
		char* filename=getinFileName(infile);
		rasimg=readTGA(infile);
		outfile=newOutFile(directory,filename,word,ext);

		//tranversing through specs and doing rotations 
		// until an unidentified specs shows up where the program will exit
		for(int i=0;i<strlen(specs);i++){
			if(specs[i]=='l' || specs[i] == 'L')
				rasimg=rotateLeft(&rasimg);
			else if (specs[i] == 'r' || specs[i] == 'R')
				rasimg=rotateRight(&rasimg);
			else{
				printf("Unknown Specification\nEnding Program\n");
				exit(0);
			}	
		}

		if (writeTGA(outfile,&rasimg)){
			printf("Writing out of the image failed.\n");
		}
	}
	free(outfile);
	return 0;
}