
/* 
  Exercise:  Counting objects and measuring object areas
*/


#define IMAGE_RANGE_CHECK

#include "stdio.h"
#include "image.h"
#include "jpegio.h"
#include "binary.h"
#include "tools.h"

int main () {
   int pixColor = COLOR_RGB(255,128,0);  // orange
   RGBImage inputImage, outputImage;
   Image<unsigned char> binary, strucElem, componentImage;

   ConnectedComponents cc;   // in binary.h, a class for connected component labelling

   int intensityThreshold = 130;   // threshold for binarizing
   int strucWidth = 5;  // width of structuring element for cleaning
   int minimumArea = 12000;   // range of size of objects to be boxed   700
   int maximumArea = 13000;  // 6000

   readJpeg( inputImage, "images/coins1.jpg" );
   int height = inputImage.height();
   int width  = inputImage.width();
   int x,y, startX, startY, gray, c;

   // create a binary image from the gray scale image using thresholding
   binary.resize( width, height );
   binary.setAll( 0 );
   for (x = 0; x < width;  x++)
   for (y = 0; y < height; y++) {
      gray = (RED(inputImage(x,y)) + GREEN(inputImage(x,y)) + BLUE(inputImage(x,y))) / 3;
      if (gray < intensityThreshold)
         binary(x,y) = 1;
   }

   // clean the binary imaage
   strucElem.resize(strucWidth,strucWidth);   // A square structuring element
   strucElem.setAll(1);
   // dilate in order to fill gaps
   binary = binaryDilation( binary, strucElem, strucWidth/2,strucWidth/2 );    

   // find all connected components.  Connectedness can be specified 
   // by FOUR_CONNECTED or EIGHT_CONNECTED
   cc.analyzeBinary( binary, EIGHT_CONNECTED );

   outputImage = cc.randomColors();    // show each component with a different color

   writeJpeg( outputImage, "images/output/object-pieces.jpg", 90 ); 
   outputImage = inputImage;
   
   printf("\n Here is the object area of each component:  ");
   for (c = 0; c < cc.getNumComponents(); c++) {
     // get the binary image of component c
     componentImage = cc.getComponentBinary( c );  
     
     int ch = componentImage.height();  // height of bounding box
     int cw = componentImage.width();   // width of bounding box
     int m,n, numPix;
     numPix = 0;

     // count the number of pixels of the component
    // numPix = number of 1-pixels of component c
     for (m = 0; m < cw; m++)
       for (n = 0; n < ch; n++)
         numPix += componentImage(m,n);

     printf("  %d",numPix);

     // if the size of the object is within the specified range
     if (minimumArea <= numPix && numPix <= maximumArea) {
       cc.getBoundary(c,startX,startY,cw,ch);

       // draw the bounding box for that component
       drawBoxRGB( outputImage, startX, startY, startX + cw, startY + ch, pixColor, 2);
     }
   }

   // write the output to a JPEG file, where the selected objects are boxed
   writeJpeg( outputImage, "images/output/objects-selected.jpg", 70 ); 
   
   printf("\nThere are %d components in the binary image. \n", cc.getNumComponents());
   printf("See the image  object-pieces.jpg  for a visualization of the components.\n");
}
