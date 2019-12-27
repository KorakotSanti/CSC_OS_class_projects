#ifndef	IMAGE_IO_TGA_H
#define	IMAGE_IO_TGA_H

#include "RasterImage.h"

RasterImage readTGA(const char* fileName,FILE *log);
int writeTGA(char* fileName, unsigned char* theData, RasterImage rasimg, FILE* log);

#endif	//	IMAGE_IO_TGA_H
