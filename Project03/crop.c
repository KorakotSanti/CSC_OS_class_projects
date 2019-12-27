#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ImageIO/imageIO_TGA.h"
#define LOG_FILE	"program.log"

RasterImage cropImage(RasterImage rasimg,unsigned int nrow, unsigned int ncol);

// function to crop image based on the new dimensions and (x,y)
RasterImage cropImage(RasterImage rasimg,unsigned int nrow, unsigned int ncol){
	unsigned char bytesPerPixel=4;

	// assigned to variable so no need to call rasimg.vairable everytime
	unsigned int row,col,xorg,yorg;
	row=rasimg.numRows;
	col=rasimg.numCols;
	xorg=rasimg.xOrigin;
	yorg=rasimg.yOrigin;

	// 
	unsigned char* croppedRas = malloc(row*col*4*sizeof(croppedRas));
	// copy the raster
	croppedRas=copyRaster(croppedRas,rasimg);

	//traverse through entire raster
	for(unsigned int i=0;i<nrow;i++){
		for(unsigned int j=0;j<ncol;j++){
			for(unsigned int os=0; os<bytesPerPixel;os++){
				// rasimg.raster is capturing only the picture within the desired
				// cropped frame
				rasimg.raster[bytesPerPixel*(i*col+j)+os] = croppedRas[bytesPerPixel*((row-nrow-yorg+i)*col+j+xorg)+os];
			}
		}
	}
	free(croppedRas);
	// change the numRows and numCol in raster image struct
	rasimg.numRows=nrow;
	rasimg.numCols=ncol;
	return rasimg;

}

int main(int argc, char** argv){

	RasterImage rasimg;

	// index for the number for output file
	int index=0;
	// word is the word that comes after the original filename
	char* word=" [cropped ";
	// the extension of the file
	char* ext=".tga";


	char *infile, *directory, *outfile;
	unsigned int xOrigin, yOrigin, width, height;
	// log for log file
	FILE* log = fopen(LOG_FILE,"a");

	// checks if the number of arguments entered is correct
	if (argc!=7) {
		printf("Not enough arguments or too much arguments\n");
		fprintf(log,"Not enough arguments or too much arguments\n");
	}
	else {
		// to write the commandline to the log file
		for(int i=0;i<argc;i++){
			fprintf(log,"%s ",argv[i]);
		}
		fprintf(log,"\n");

		// converting in line argument from string to int and storing them into 
		// variables
		sscanf(argv[1],"%d",&xOrigin);
		sscanf(argv[2],"%d",&yOrigin);
		sscanf(argv[3],"%d",&width);
		sscanf(argv[4],"%d",&height);
		infile=argv[5];
		directory=argv[6];

		// reading TGA and storing image information into RasterImage struct
		rasimg=readTGA(infile,log);
		// getting the filename without the extension
		char* filename= getinFileName(infile);

		// infintely loop
		while(1){
			// allocate enough space to contain the number
			char* num=malloc(sizeof(char)*10);
			//convert int to char*
			sprintf(num,"%d",index++);

			// allocate space for newword
			char* newword=malloc(sizeof(char)*(strlen(word)+strlen(num)+2));
			// concating all the necessary strings for the end of the filename
			strcpy(newword,word);
			strcat(newword,num);
			strcat(newword,"]");
			// get the output file 
			outfile=newOutFile(directory,filename,newword,ext);

			// free variable
			free(num);
			free(newword);
			// break loop when found the outfile that does not exists
			if(!fopen(outfile,"r")){
				break;
		}
	}
		//storing x orign to the raster struct
		rasimg.xOrigin=xOrigin;
		rasimg.yOrigin=yOrigin;

		// get new RasterImage with new height and width
		rasimg=cropImage(rasimg,height,width);
		if (writeTGA(outfile, rasimg.raster, rasimg,log)){
			printf("Writing out of the image failed.\n");
			fprintf(log,"Writing out of the image failed.\n");
		}

	}
	fclose(log);
	free(outfile);
	return 0;
}