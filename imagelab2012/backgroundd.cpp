#define IMAGE_RANGE_CHECK

#include "stdio.h"
#include "image.h"
#include "jpegio.h"
#include "binary.h"

void neutral_color(RGBImage & input);

main(){
//blue-red litmus paper
//red-blue litmus paper
//get standard color from ph scale
//determine what is the color of the litmus paper
RGBImage inputImage;
char filename[30];

readJpeg( inputImage, "images/substances/img0001.jpg" );
neutral_color(inputImage);
}

//neutral color litmus paper
void neutral_color(RGBImage & input){
 char filename[30];
 int width, height, i, j, gray, bins[256];
 for(i=0; i<256; i++)bins[i]=0;
 width=input.width();
 height=input.height();
 for(i=0; i<width; i++)
 {
	for(j=0; j<height;j++)
	{
		gray=(RED(input(i, j))+GREEN(input(i,j))+BLUE(input(i,j)))/3;
		//if((gray>=220&&gray<=255))input(i, j)=COLOR_RGB(255,0,0);
		bins[gray]++;	
	}
 }
 //sprintf( filename, "images/ph/test1.jpg"); 
 //writeJpeg( input, filename, 90 );
 for(i=0; i<256; i++)printf("| %d-%d |\n",i, bins[i]);
}
