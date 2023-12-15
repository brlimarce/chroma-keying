
/* 
  Page layout analysis using iterative grouping of document objects
  Characters --> words --> lines --> paragraphs --> columns
*/


#define IMAGE_RANGE_CHECK

#include "stdio.h"
#include "image.h"
#include "jpegio.h"
#include "binary.h"

int main () {
   int pixColor = COLOR_RGB(255,128,0);  // orange
   RGBImage inputImage, outputImage;
   Image<unsigned char> binary, binary2, strucElem, componentImage;
   char fileName[200];
   int group;
   
   ConnectedComponents cc;   // in binary.h, a class for connected component labelling

   int intensityThreshold = 130;   // threshold for binarizing
   int strucWidth, strucHeight;  // dimensions of structuring element for cleaning and smearing
   int minimumArea = 10;   // minimum size of objects to be considered, anything smaller is noise 

   readJpeg( inputImage, "images/journal1.jpg" );
   int height = inputImage.height();
   int width  = inputImage.width();
   int x,y, startX, startY, gray, c;

   // create a binary image from the gray scale image using thresholding
   binary.resize( width, height );
   binary.setAll( 0 );
   for (x = 0; x < width;  x++) {
     for (y = 0; y < height; y++) {
       gray = (RED(inputImage(x,y)) + GREEN(inputImage(x,y)) + BLUE(inputImage(x,y))) / 3;
       if (gray < intensityThreshold)
         binary(x,y) = 1;
     }
   }

  for (group = 0; group < 4; group++) {
    switch (group) {
      // filling of very small gaps
      case 0: strucWidth = 1; strucHeight = 2; break;  
      // horizontal smearing:  grouping characters into words
      case 1: strucWidth = 5; strucHeight = 1; break;  
      // horizontal smearing:  grouping words into lines
      case 2: strucWidth = 10; strucHeight = 1; break;  
      // vertical smearing:  grouping lines into paragraphs
      case 3: strucWidth = 1; strucHeight = 6; break;  
    }
    
    strucElem.resize(strucWidth,strucHeight);   // A square structuring element
    strucElem.setAll(1);
    binaryDilation( binary2, binary, strucElem, strucWidth/2,strucHeight/2 );    

    // find all connected components.  Connectedness can be specified by FOUR_CONNECTED or EIGHT_CONNECTED
    cc.analyzeBinary( binary2, EIGHT_CONNECTED );

    outputImage = cc.randomColors();    // show each component with a different color
    sprintf( fileName, "images/output/object-pieces-%d.jpg",group);
    writeJpeg( outputImage, fileName, 100 ); 
    outputImage = inputImage;
   
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
      // if the size of the object is greater than noise particles
      if (minimumArea <= numPix) {
        // get the component's bounding box's top-left corner and dimensions
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
      }
    }
    sprintf( fileName, "images/output/object-group-%d.jpg",group);
    writeJpeg( outputImage, fileName, 100 ); 
  }
}
