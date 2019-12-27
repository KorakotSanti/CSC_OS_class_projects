#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ImageIO/imageIO_TGA.h"
#define LOG_FILE	"program.log"

unsigned char* alterRaster(RasterImage rasimg,int index);

/*
this function kills all the color chanell except for the channel at index
*/
unsigned char* alterRaster(RasterImage rasimg,int index){

	unsigned char* tempras = malloc(rasimg.numCols*rasimg.numRows*4*sizeof(tempras)+1);
	tempras=copyRaster(tempras,rasimg);

	unsigned char bytesPerPixel=4;

	// loops through raster killing color channels
	for (unsigned int i=0; i< rasimg.numRows; i++)
	{
		for (unsigned int j=0; j< rasimg.numCols; j++)
		{	
			for(int l=0;l<4;l++){
				if(l!=index)
					tempras[bytesPerPixel*(i*rasimg.numCols+j)+l] = 0x00;
			}
		}
	}

	return tempras;
}

int main(int argc, char** argv){

	RasterImage rasimg;

	// log for log file
	FILE* log = fopen(LOG_FILE,"a");
	// color to represent the colors
	char *color[3];
	color[0]="_r";
	color[1]="_g";
	color[2]="_b";
	char *ext=".tga";
	char *outfile;
	unsigned char *tempras;

	// checks if have enough arguments
	if (argc!=3) {
		printf("Not enough arguments or too much arguments\n");
		fprintf(log,"Not enough arguments or too much arguments\n");
	}
	else {
		// to write the commandline to the log file
		for(int i=0;i<argc;i++){
			fprintf(log,"%s ",argv[i]);
		}
		fprintf(log,"\n");
		
		//storing arguments in variables
		char* infile=argv[1];
		char* directory=argv[2];
		char* filename= getinFileName(infile);
		// reading infile and storing them in RasterImage
		rasimg=readTGA(infile,log);

		// tranverse 3 times for red, blue, and green, seperating the color channel
		for(int i=0;i<3;i++){
			outfile=newOutFile(directory,filename,color[i],ext);
			tempras=alterRaster(rasimg,i);
			if (writeTGA(outfile, tempras, rasimg,log)){
				printf("Writing out of the image failed.\n");
				fprintf(log,"Writing out of the image failed.\n");
			}
			free(tempras);
		}
	}
	free(outfile);
	free(rasimg.raster);
	fclose(log);
	return 0;
}