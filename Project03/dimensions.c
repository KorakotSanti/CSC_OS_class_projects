#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ImageIO/imageIO_TGA.h"

// LOG FILE TO ADD TO 
#define LOG_FILE	"program.log"

void getDimension(RasterImage rasimg,FILE *log);
void getDimensionMod(char* mod, RasterImage rasimg,FILE *log);
void getDimensionMod2(char* mod, char* mod2, RasterImage rasimg, FILE *log);

//the basic dimension function, that prints the width x height
void getDimension(RasterImage rasimg,FILE *log) {
	printf("%d %d \n",rasimg.numCols,rasimg.numRows);
	fprintf(log,"%d %d\n",rasimg.numCols,rasimg.numRows);
}

// dimesion if there is only one modifier
// the fuction will print out the width,height depending on modifier 
// and log what was print out to the log file
void getDimensionMod(char* mod, RasterImage rasimg,FILE *log) {

	if(!strcmp(mod,"-v")) {
		printf("Width:	%d, Height:	%d \n",rasimg.numCols,rasimg.numRows);
		fprintf(log,"Width:	%d, Height: %d\n",rasimg.numCols,rasimg.numRows);
	}
	else if (!strcmp(mod,"-h")) {
		printf("%d\n",rasimg.numRows);
		fprintf(log,"%d\n",rasimg.numRows);
	}
	else if (!strcmp(mod,"-w")){
		printf("%d\n",rasimg.numCols);
		fprintf(log,"%d\n",rasimg.numCols);
	}
	else {
		printf("Unidentified Modifer\n");
		fprintf(log,"Unidentified Modifer\n");
	}
}

// this function is same as getDimensionMod Function but takes 2 modifiers instead
void getDimensionMod2(char* mod, char* mod2, RasterImage rasimg, FILE *log){

	if (!strcmp(mod,"-v") || !strcmp(mod2,"-v")){
		if (!strcmp(mod,"-h") || !strcmp(mod2,"-h")){
			printf("Height: %d\n",rasimg.numRows);
			fprintf(log,"Height: %d\n",rasimg.numRows);
		}
		else if (!strcmp(mod,"-w") || !strcmp(mod2,"-w")){
			printf("Width: %d\n",rasimg.numCols);
			fprintf(log,"Width: %d\n",rasimg.numCols);
		}
		else{
			printf("Unidentified Modifers\n");
			fprintf(log,"Unidentified Modifers\n");
		}
	}
	else {
		printf("Unidentified Modifers\n");
		fprintf(log,"Unidentified Modifers\n");
	}
}
int main(int argc, char** argv){
	// infile represents file to read from
	// mod and mod2 represents the modifiers
	char* infile, *mod, *mod2;
	RasterImage rasimg;

	// open log file to add to it
	FILE* log = fopen(LOG_FILE,"a");

	// if else block check the number of arguement before proceeding 
	// if not enough argument or too much argument error message occurs and write to log
	// otherwise will store arguements to correct variable
	if (argc==1){
		printf("Not Enough Agruments\n You must enter at least 1 Argument!");
		fprintf(log,"Not Enough Arguments\n You must enter at least 1 Argument!");
	}
	else if (argc==2){
		// to write the commandline to the log file
		for(int i=0;i<argc;i++){
			fprintf(log,"%s ",argv[i]);
		}
		fprintf(log,"\n");

		infile=argv[1];
		rasimg=readTGA(infile,log);
		getDimension(rasimg,log);
	}
	else if (argc==3){
		// to write the commandline to the log file
		for(int i=0;i<argc;i++){
			fprintf(log,"%s ",argv[i]);
		}
		fprintf(log,"\n");

		mod = argv[1];
		infile = argv[2];
		rasimg = readTGA(infile,log);
		getDimensionMod(mod,rasimg,log);
	}
	else if (argc==4){

		// to write the commandline to the log file
		for(int i=0;i<argc;i++){
			fprintf(log,"%s ",argv[i]);
		}
		fprintf(log,"\n");

		mod=argv[1];
		mod2=argv[2];
		infile=argv[3];
		rasimg = readTGA(infile,log);
		getDimensionMod2(mod,mod2,rasimg,log);
	}
	else {
		printf("Too Much Arguments\n You must enter no more than 3 Arguments!");
		fprintf(log,"Too Much Arguments\n You must enter no more than 3 Arguments!");
	}
	free(rasimg.raster);
	fclose(log);

	return 0;

}