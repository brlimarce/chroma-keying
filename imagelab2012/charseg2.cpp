
/* 
  A Demo of Morphological Operations and Connected Component Analysis
      
  The code for the functions are in binary.h and binary.cpp.
  Check these files for other available functions.

*/


#define IMAGE_RANGE_CHECK

#include "stdio.h"
#include "stdlib.h"
#include "image.h"
#include "jpegio.h"
#include "binary.h"

//------------------------------------------------------------
//  The program reads in  train1.jpg
//  and saves each segmented character in a separate image
//------------------------------------------------------------

int main () {
   RGBImage inputImage, outputImage, charImage;
   Image<unsigned char> binary, binary2, strucElem, componentImage;

   ConnectedComponents cc;   // a class for connected component labelling
   char charFileName[30];
   
   int threshold = 130;  // threshold for binarizing the document image
   readJpeg( inputImage, "images/digits.jpg" );
   int height = inputImage.height();
   int width  = inputImage.width();
   int x,y, gray, c;

   // create a binary image from the document image using thresholding
   binary.resize( width, height );
   binary.setAll( 0 );

   binary2.resize(width, height);
   binary2.setAll(0);

   for (x = 0; x < width;  x++)
   for (y = 0; y < height; y++) {
      gray = (RED(inputImage(x,y)) + GREEN(inputImage(x,y)) + BLUE(inputImage(x,y))) / 3;
      if (gray < threshold) {
         binary(x,y) = 1;
         binary2(x, y) = 1;
      }
   }

   strucElem.resize(3,3);     // 3x3 square structuring element
   strucElem.setAll(1);
   binaryDilation( binary2, binary, strucElem, 1,1 );    // dilate in order to fill gaps

   // find all connected components.  Connectedness can be specified 
   // by FOUR_CONNECTED or EIGHT_CONNECTED
   cc.analyzeBinary( binary2, EIGHT_CONNECTED );  // connected component labelling

   outputImage = cc.randomColors();    // show each component with a different color

   printf("The image containing the colored components can be found in...\n");
   writeJpeg( outputImage, "images/output/pieces.jpg", 70 ); 

   // save the individual character images in a separate directory
   system("mkdir images/output/charseg");
   
   // for each connected component c
   for (c = 0; c < cc.getNumComponents(); c++) {
     // get the binary image of component c
     componentImage = cc.getComponentBinary( c );  
     
     int w,h,m,n;
     
     cc.getBoundary(c, x, y, w, h);
     charImage.resize(w*2,h);

     // draw the character image:   Raw gray image on the right, and binary image on the left
     for (m = 0; m < w; m++)  {
       for (n = 0; n < h; n++) {
         charImage(m,n) = (componentImage(m,n) == 0) ? COLOR_RGB(255,255,255) : COLOR_RGB(0,0,0);
         charImage(w+m,n) = inputImage(x+m,y+n);
       }
     }
     // save the image of the isolated character
     sprintf( charFileName, "images/output/charseg/%04d.jpg", c ); 
     writeJpeg( charImage, charFileName, 90 );
   }
   printf("\n\n There are %d components in the binary image. \n", cc.getNumComponents());
}

