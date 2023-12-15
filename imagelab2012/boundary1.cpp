
/* 
     Tracing the boundary of each object
*/

#define IMAGE_RANGE_CHECK

#include "stdio.h"
#include "image.h"
#include "jpegio.h"
#include "binary.h"
#include "stdlib.h"



int main () {

   int pixColor = COLOR_RGB(255,128,0);  // orange
   RGBImage inputImage, outputImage, boundaryRGB, objectRGB;
   Image<unsigned char> binary, binary2, strucElem, componentImage;
   int boundStartX, boundStartY;
   char fileName[200];
   
   ConnectedComponents cc;   // in binary.h, a class for connected component labelling

//   int intensityThreshold = 130;   // threshold for binarizing
   int strucWidth = 5;  // width of structuring element for cleaning
   int minimumArea = 20;   // range of size of objects to be considered
   int maximumArea = 1000000;
   int perimeter;

//   int intensityThreshold = 130;   // threshold for binarizing
//   readJpeg( inputImage, "images/Azucena_600_01.jpg");

   int intensityThreshold = 180;   // threshold for binarizing
   readJpeg( inputImage, "images/Rayada_600_wtScnbg.jpg");
//   readJpeg( inputImage, "images/hand7.jpg" );
   int height = inputImage.height();
   int width  = inputImage.width();
   int x,y, startX, startY, gray, c;

   // create a binary image from the gray scale image using thresholding
   binary.resize( width, height );
   binary.setAll( 0 );
   for (x = 5; x < width-5;  x++)
   for (y = 5; y < height-5; y++) {
      gray = (RED(inputImage(x,y)) + GREEN(inputImage(x,y)) + BLUE(inputImage(x,y))) / 3;
      if (gray < intensityThreshold)
         binary(x,y) = 1;
   }

   // clean the binary imaage
   strucElem.resize(strucWidth,strucWidth);   // A square structuring element
   strucElem.setAll(1);
   // dilate in order to fill gaps
   binaryDilation( binary2, binary, strucElem, strucWidth/2,strucWidth/2 );    

   // find all connected components.  Connectedness can be specified 
   // by FOUR_CONNECTED or EIGHT_CONNECTED
   cc.analyzeBinary( binary2, EIGHT_CONNECTED );

   outputImage = cc.randomColors();    // show each component with a different color

   system("mkdir images/output/boundary");
   writeJpeg( outputImage, "images/output/boundary/object-pieces.jpg", 90 ); 
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

     printf("Component %d:  area = %d\n",c,numPix);

     // if the size of the object is within the specified range
     if (minimumArea <= numPix && numPix <= maximumArea) {
       cc.getBoundary(c,startX,startY,cw,ch);

       // draw the bounding box for that component
       // draw the left and right boundaries
       for (y = startY; y <= startY+ch; y++) {
         outputImage(startX,y) = outputImage(startX+1,y+1) = pixColor;   // make the line 2-pixel thick
         outputImage(startX+cw,y) = outputImage(startX+cw+1,y+1) = pixColor;   // make the line 2-pixel thick
       }
       // draw the top and bottom boundaries
       for (x = startX; x <= startX+cw;  x++) {
         outputImage(x,startY) = outputImage(x+1,startY+1) = pixColor;   // make the line 2-pixel thick
         outputImage(x,startY+ch) = outputImage(x+1,startY+ch+1) = pixColor;  // make the line 2-pixel thick
       }
       // find a  1-pixel on the object's boundary
       boundStartX = 0;  boundStartY = 0;
       while (componentImage(boundStartX,boundStartY) == 0) {
         boundStartX++;
       }
       // trace the object's boundary 
       binaryFollowBoundary(boundaryRGB, (float &)perimeter, componentImage,boundStartX,boundStartY);
       objectRGB.resize(cw*2,ch);
       for (x = 0; x < cw; x++) {
         for (y = 0; y < ch; y++) {
           objectRGB(x,y) = inputImage(startX+x,startY+y);
           objectRGB(cw+x,y) = boundaryRGB(x,y);           
         }
       }
       sprintf( fileName, "images/output/boundary/object-%03d-boundary.jpg", c);
       writeJpeg( objectRGB, fileName, 100 ); 
     }
   }

   // write the output to a JPEG file, where the selected objects are boxed
   writeJpeg( outputImage, "images/output/boundary/objects-selected.jpg", 70 ); 
   
   printf("\nThere are %d components in the binary image. \n", cc.getNumComponents());
   printf("See the image boundary/object-pieces.jpg  for a visualization of the components.\n");
}
