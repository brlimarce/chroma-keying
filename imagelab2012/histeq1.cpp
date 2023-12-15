
/* 
  Exer:  Adjust the contrast of a gray scale image using histogram equalization
*/


#define IMAGE_RANGE_CHECK

#include "image.h"
#include "jpegio.h"
#include "stdio.h"
#include "filter.h"

int main () {
   int x,y, pix;
   RGBImage inputImage, outputImage;
   Image<unsigned char> gray, histeq;
   char outputFileName[100];
   int percent = 67;
   // read the JPEG file
   readJpeg( inputImage, "images/hill.jpg" );

   int height = inputImage.height();
   int width  = inputImage.width();
   
   int startX = 0;
   int startY = 0;
   int endX = width-1;
   int endY = height-1;

   gray.resize( width,height );  
   // compute the gray scale of the sub-image
   for (y = startY; y < endY; y++) {
     for (x = startX; x < endX;  x++) {
       pix = inputImage(x,y);
       gray(x,y) = (RED(pix) + GREEN(pix) + BLUE(pix)) / 3;
     }
   }

  histogramEqualize(histeq,gray,startX,startY,endX,endY,percent);

  outputImage.resize( width,height );
  for (y = startY; y < endY; y++) {
     for (x = startX; x < endX;  x++) {
       outputImage(x,y) = COLOR_RGB(histeq(x,y),histeq(x,y),histeq(x,y));
     }
  }
  // write the output image to a JPEG file
  sprintf( outputFileName, "images/output/histeq%03d.jpg", percent);
  writeJpeg( outputImage, outputFileName, 80 ); 
       // the last parameter is JPEG quality (1..100 = best)
}
