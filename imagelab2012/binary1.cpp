
/* 
  Creating a binary image using an intensity threshold
*/

#define IMAGE_RANGE_CHECK

#include "stdio.h"
#include "image.h"
#include "jpegio.h"
#include "binary.h"

int main () {
   RGBImage inputImage, outputImage;
   Image<unsigned char> binary;

   int intensityThreshold = 150;

   readJpeg( inputImage, (char *) "images/objects.jpg" );
   int height = inputImage.height();
   int width  = inputImage.width();
   int x,y, gray;

   // create a binary image using thresholding
   binary.resize( width, height );
   binary.setAll( 0 );   // set all to background

   // create an RGB visualization of the binary image
   outputImage.resize( width, height );
   outputImage.setAll( COLOR_RGB(255,255,255) );  // set all to white

   for (x = 0; x < width;  x++) {
     for (y = 0; y < height; y++) {
       gray = (RED(inputImage(x,y)) + GREEN(inputImage(x,y)) + BLUE(inputImage(x,y))) / 3;
       if (gray < intensityThreshold) {   // if the pixel is dark
         binary(x,y) = 1;   // this pixel is part of the object
         outputImage(x,y) = COLOR_RGB(0,0,0);  // in the visualization, it will be colored black
       }
     }
   }

   // write the output to a JPEG file
   system("mkdir images/output");
   writeJpeg( outputImage, (char *) "images/output/binary.jpg", 100 ); 
}
