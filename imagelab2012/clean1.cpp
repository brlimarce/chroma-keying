
/* 
    Cleaning binary images
*/


#define IMAGE_RANGE_CHECK

#include "stdio.h"
#include "image.h"
#include "jpegio.h"
#include "binary.h"


int main () {

   int intensityThreshold = 130;
   int strucWidth = 7;   // width of the SQUARE structuring element

   RGBImage inputImage, outputImage1, outputImage2;
   Image<unsigned char> binary1, binary2, binary3, strucElem;

   readJpeg( inputImage, "images/objects.jpg" );
   int height = inputImage.height();
   int width  = inputImage.width();
   int x,y, startX, startY, gray, c;

   // create a binary image from the image using thresholding
   binary1.resize( width, height );
   binary1.setAll( 0 );

   outputImage1.resize( width, height );
   outputImage1.setAll( COLOR_RGB(255,255,255) );  // set all to white
   outputImage2 = outputImage1;

   // separate the object pixels from the background using a simple intensity threshold
   // this assumes the background has a much lighter intensity than the objects
   for (x = 0; x < width;  x++) {
     for (y = 0; y < height; y++) {
       gray = (RED(inputImage(x,y)) + GREEN(inputImage(x,y)) + BLUE(inputImage(x,y))) / 3;
       if (gray < intensityThreshold) {
         binary1(x,y) = 1;    // object  pixel
         outputImage1(x,y) = COLOR_RGB(0,0,0);  // color black
       }
     }
   }

   strucElem.resize(strucWidth,strucWidth);   // A square structuring element
   strucElem.setAll(1);

   // dilation fills in gaps
   //  strucWidth/2,strucWidth/2   specifies the center of the structuring element 
   binary2 = binaryDilation( binary1,strucElem,strucWidth/2,strucWidth/2 ); 
   // erosion is effective in reducing noise
  binary3 = binaryErosion(binary2,strucElem,strucWidth/2,strucWidth/2 );    

   // visualize the resulting binary image in a color image
   for (x = 0; x < width;  x++) {
     for (y = 0; y < height; y++) {
       if (binary3(x,y)) {
         outputImage2(x,y) = COLOR_RGB(0,0,0);
       }
     }
   }

   // write the output to a JPEG file
   writeJpeg( outputImage1, "images/output/binary.jpg", 70 ); 
   writeJpeg( outputImage2, "images/output/cleaned.jpg", 70 ); 
}
