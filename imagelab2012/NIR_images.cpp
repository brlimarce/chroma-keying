/*
1. Convert to grayscale
2. apply tophat

*/

#define IMAGE_RANGE_CHECK

#include "image.h"
#include "jpegio.h"
#include "tools.h"
#include "stdio.h"
#include "color.h"


int main (int argc, char *argv[]) {
	RGBImage grayImage, sourceImage, tophatImage;
	Image<unsigned char> gray, opening, tophat;
	
	long int distance;
	int height, width, grayh, grayw;
	int x,y,i,j,pix;
	int radius,minimum;      //tophat radius
	
	readJpeg(sourceImage, argv[1]);
	sscanf(arg[2],"%i", &radius);
	
	height = sourceImage.height();
	width = sourceImage.width();
	
	gray.resize(width+2*radius,height+2*radius);
	gray.setAll(255);
	
	grayw = gray.width();
	grayh = gray.height();
	
	grayImage.resize(width,height);
	tophatImage.resize(width,height);
	open.resize(width, height);
	makeGrayfromRGB ( grayImage, sourceImage );

	writeJpeg(grayImage, "	
	for ( y = 0 ; y < height; y++ )
	for ( x = 0 ; x < wdith ; x++ ) {
		pix = grayImage(x,y);
		gray(x+radius,y+radius) = RED(pix);
	}
	
	for ( y = radius ; y < width+radius; y++ )
	for ( x = radius ; x < height+radius ; x++ ) {
		minimum = 255;
		for ( i = x - radius ; i < x + radius ; i++)
		for ( j = y - radius ; j < y + radius ; j++) {
			distance = (i-x)*(i-x) + (j-y)*(j-y);
			if ( distance <= radius*radius ) 
				if (minimum > gray(i,j) )
					minimum = gray(i,j);
		}
		
			
	}
}