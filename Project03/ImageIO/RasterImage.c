#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "RasterImage.h"

// this function returns the newoutfile from an directory
char* newOutFile(char* directory, char* filename, char *chr, char* type){
	// to allocate enough space for the concatenate all the strings together
	char* result = (char*)malloc(strlen(directory)+strlen(filename)+strlen(type)+strlen(chr)+1);

	// first copy the first string into the allocated string
	strcpy(result,directory);

	// then concatenate the rest
	strcat(result,filename);
	strcat(result,chr);
	strcat(result,type);
	return result;
}

/*
Function to get the filename from a path
*/
char* getinFileName(char* infile){
	// allocate enough space for a duplicate of infile
	char* result=(char*)malloc(strlen(infile)+1);

	//copy infile
	strcpy(result,infile);

	// traverse through loop at the end getting the filename
	for (int i=0;i<strlen(infile);i++){
		if (result[i]=='/')
			result=result+i;
	}

	// sperating extension
	strtok(result,".");
	return result;
}

/* 
function to copy the raster by manual storing it to each part of the array
*/
unsigned char* copyRaster(unsigned char* tempras, RasterImage rasimg){
	unsigned char bytesPerPixel=4;

	for (unsigned int i=0; i< rasimg.numRows; i++)
	{
		for (unsigned int j=0; j< rasimg.numCols; j++)
		{	
			for(int l=0;l<bytesPerPixel;l++){
				tempras[bytesPerPixel*(i*rasimg.numCols+j)+l] = rasimg.raster[bytesPerPixel*(i*rasimg.numCols+j)+l];
			}
		}
	}

	return tempras;
}


