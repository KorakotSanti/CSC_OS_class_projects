#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ImageIO/imageIO_TGA.h"
#define LOG_FILE	"program.log"

unsigned char* rotateLeft(RasterImage rasimg);
unsigned char* rotateRight(RasterImage rasimg);

// this function returns a new raster of the rotated image to the left
unsigned char* rotateLeft(RasterImage rasimg) {

	//allocate for new Raster
	unsigned char* newRas = malloc(rasimg.numRows*rasimg.numCols*4*sizeof(newRas));

	unsigned char bytesPerPixel=4;

	unsigned int row = rasimg.numRows;
	unsigned int col = rasimg.numCols;

	// tranverse through all the of the raster and the pixels that stores the color channels
	for(unsigned int i=0;i<row;i++){
		for(unsigned int j=0;j<col;j++){
			for(unsigned int os=0;os<bytesPerPixel;os++){
				newRas[bytesPerPixel*(j*row+row-i-1)+os] = rasimg.raster[bytesPerPixel*(i*col+j)+os];
			}
		}
	}
	return newRas;
}

//same as rotate Left function except it rotates right
// all the logic is same except for storing the values
unsigned char* rotateRight(RasterImage rasimg) {
	unsigned char* newRas = malloc(rasimg.numRows*rasimg.numCols*4*sizeof(newRas));

	unsigned char bytesPerPixel=4;

	unsigned int row = rasimg.numRows;
	unsigned int col = rasimg.numCols;

	for(unsigned int i=0;i<row;i++){
		for(unsigned int j=0;j<col;j++){
			for(unsigned int os=0;os<bytesPerPixel;os++){
				newRas[bytesPerPixel*(row*(col-1-j)+i)+os] = rasimg.raster[bytesPerPixel*(i*col+j)+os];
			}
		}
	}
	return newRas;
}

int main(int argc, char** argv){

	RasterImage rasimg;

	// log for log file
	FILE* log = fopen(LOG_FILE,"a");
	// color to represent the colors
	char *word=" [rotated]";
	char *ext=".tga";
	char *outfile;
	unsigned char *rotatedRas;


	if (argc!=4) {
		printf("Not enough arguments or too much arguments\n");
		fprintf(log,"Not enough arguments or too much arguments\n");
	}
	else {
		for(int i=0;i<argc;i++){
			fprintf(log,"%s ",argv[i]);
		}
		fprintf(log,"\n");

		char* specs = argv[1];
		char* infile =argv[2];
		char* directory = argv[3];
		char* filename=getinFileName(infile);
		rasimg=readTGA(infile,log);
		outfile=newOutFile(directory,filename,word,ext);

		//tranversing through specs and doing rotations 
		// until an unidentified specs shows up where the program will exit
		for(int i=0;i<strlen(specs);i++){
			if(specs[i]=='l' || specs[i] == 'L')
				rasimg.raster=rotateLeft(rasimg);
			else if (specs[i] == 'r' || specs[i] == 'R')
				rasimg.raster=rotateRight(rasimg);
			else{
				printf("Unknown Specification\nEnding Program\n");
				fprintf(log,"Unknown Specification\n Ending Program\n");
				exit(0);
			}
			// switch the height and weight for each rotation		
			unsigned int temp= rasimg.numRows;
			rasimg.numRows=rasimg.numCols;
			rasimg.numCols=temp;
		}


		if (writeTGA(outfile, rasimg.raster, rasimg,log)){
			printf("Writing out of the image failed.\n");
			fprintf(log,"Writing out of the image failed.\n");
		}
	}
	free(outfile);
	free(rasimg.raster);
	fclose(log);
	return 0;
}