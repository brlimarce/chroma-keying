
/* 
  A Demo of Morphological Operations and Connected Component Analysis
  by Vladimir Y. Mariano, Institute of Computer Science
    
  The code for the functions are in binary.h and binary.cpp.
  Check these files for other available functions.

*/


#define IMAGE_RANGE_CHECK

#include "stdio.h"
#include "image.h"
#include "jpegio.h"
#include "binary.h"

//------------------------------------------------------------
//  The program reads in a "digits" JPEG image
//  and colors each connected component with a unique color
//------------------------------------------------------------

int main () {
   RGBImage inputImage, outputImage;
   Image<unsigned char> binary, strucElem, componentImage;

   ConnectedComponents cc;   // a class for connected component labelling

   readJpeg( inputImage, "digits.jpg" );
   int height = inputImage.height();
   int width  = inputImage.width();
   int x,y, gray, c;
   int threshold = 128;

   // create a binary image from the document image using thresholding
   binary.resize( width, height );
   binary.setAll( 0 );
   for (x = 0; x < width;  x++)
   for (y = 0; y < height; y++) {
      gray = (RED(inputImage(x,y)) + GREEN(inputImage(x,y)) + BLUE(inputImage(x,y))) / 3;
      if (gray < threshold)
         binary(x,y) = 1;
   }

   strucElem.resize(3,3);     // 3x3 square structuring element
   strucElem.setAll(1);
   binary = binaryDilation( binary, strucElem, 1,1 );    // dilate in order to fill gaps

   // find all connected components.  Connectedness can be specified 
   // by FOUR_CONNECTED or EIGHT_CONNECTED
   cc.analyzeBinary( binary, EIGHT_CONNECTED );

   outputImage = cc.randomColors();    // show each component with a different color

   writeJpeg( outputImage, "pieces.jpg", 70 ); 

   printf("\n Here is the object area of each component:  ");
   for (c = 0; c < cc.getNumComponents(); c++) {
     // get the binary image of component c
     componentImage = cc.getComponentBinary( c );  
     
     int ch = componentImage.height();
     int cw = componentImage.width();
     int m,n, numPix;
     numPix = 0;
     for (m = 0; m < cw; m++)
       for (n = 0; n < ch; n++)
         numPix += componentImage(m,n);

     printf("%d, ", numPix );
   }
   printf("\n\n There are %d components in the binary image. \n", cc.getNumComponents());
   printf("See the image  pieces.jpg  for a visualization of the components.\n");
}

