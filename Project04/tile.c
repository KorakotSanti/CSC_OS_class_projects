
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//
#include "ImageIO/imageIO_TGA.h"
/** Function to get the tile height and width which means the amount of
*   images that will fit in the row and column
*	@param imgperwidth		num of image on each row
*	@param imgperheight     num of image on each column
*	@param numImage			num of total images	
*/
void getnewDimensions(unsigned int *imgperwidth,unsigned int *imgperheight,unsigned numImage);

/** function to create the composite image
*	@param inputImages		list of raster image struct with information on each of the input image
*	@param height           height of each individual input image
*	@param width            width of each individual input image
*	@param imgperwidth      num of image on each row
*	@param imgperheight     num of image on each column
*   @param numImage			num of total input image
*	return RasterImage struct with hold the information on the composite image
*/
RasterImage compositeImage(RasterImage* inputImg,unsigned int height,
							unsigned int width, unsigned int imgperwidth, 
							unsigned int imgperheight, unsigned int numImage);

void getnewDimensions(unsigned int *imgperwidth,unsigned int *imgperheight,unsigned numImage){
	*imgperwidth=0;
	*imgperheight=0;

	// loop until get the right dimension to fit all input image
	while(1){
		*imgperwidth+=1;
		*imgperheight+=1;

		if ((*imgperwidth)*(*imgperheight)==numImage){
			break;
		}
		else if ((*imgperwidth)*(*imgperheight-1)==numImage){
			*imgperheight-=1;
			break;
		}
		else if (*imgperwidth*(*imgperheight-1)<numImage && (*imgperwidth)*(*imgperheight)>numImage){
			break;
		}
		else if ((*imgperwidth)*(*imgperheight-1)>numImage && (*imgperwidth)*(*imgperheight-2)<numImage){
			*imgperheight-=1;
			break;
		}
	}
}

RasterImage compositeImage(RasterImage* inputImg,unsigned int height,
							unsigned int width, unsigned int imgperwidth, 
							unsigned int imgperheight, unsigned int numImage){

	unsigned int htile=0,wtile=0;
	// new image with new width and height
	RasterImage outimg=newRasterImage(height*imgperheight,width*imgperwidth,RGBA32_RASTER);

	// outer most loop is a loop that loop through number of total input images 
	for(unsigned int i=0;i<numImage;i++){
		for(unsigned int h=0;h<height;h++){
			for(unsigned int w=0;w<width;w++){
				memcpy((unsigned char*) outimg.raster+(outimg.numRows-h-1-height*htile)*outimg.bytesPerRow+(w+width*wtile)*outimg.bytesPerPixel,
						(unsigned char*) inputImg[i].raster + (inputImg[i].numRows-h-1)*inputImg[i].bytesPerRow+w*inputImg[i].bytesPerPixel,
						outimg.bytesPerPixel);
			}
		}
		// increment wtile for the x value
		wtile++;

		// purpose to check if wtile reached the max tile
		if(wtile==imgperwidth){
			wtile=0;
			htile++;
		}
	}

	return outimg;
}
int main(int argc, char* argv[]){

	// check for arguments
	if (argc<3){
		printf("Error\n");
		printf("Proper Usage: tile list_of_path_to_image_file path_to_output_image");
		return 1;
	}


	unsigned int width,height,numImage=argc-2;
	// outputimage path is always the very last item in argv
	char* outputimage = argv[argc-1];

	// list of input images
	RasterImage* inputImages = (RasterImage*)calloc(numImage,sizeof(RasterImage*));

	// reading of all input images and putting into the array
	for (int i =1; i<argc-1;i++){
		inputImages[i]=readTGA(argv[i]);
	}

	// get the height and width of all inputimages which share the same dimensions
	height=inputImages[0].numRows;
	width=inputImages[0].numCols;

	unsigned int imgperheight,imgperwidth;

	// get the new composite image height and width
	getnewDimensions(&imgperwidth,&imgperheight,numImage);

	// create the new image and write it 
	RasterImage img=compositeImage(inputImages,height,width,imgperwidth,imgperheight,numImage);
	writeTGA(outputimage,&img);
	return 0;
}