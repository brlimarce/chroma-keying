
/* 
   Draw a box on an image canvass
    
*/


#define IMAGE_RANGE_CHECK

#include "image.h"
#include "jpegio.h"


int main () {
   int x,y, height,width;

   int pixColor = COLOR_RGB(255,128,0);  // color of the box
   int startX = 120;    // bounding box of the gray part
   int endX = 225;
   int startY = 15;
   int endY = 100;

   RGBImage inputimage, outputimage;

   // read the JPEG file
   readJpeg( inputimage, "images/kristen.jpg" );
   height = inputimage.height();
   width  = inputimage.width();

   // copy the output image to input image
   outputimage = inputimage;

   // draw the left and right boundaries
   for (y = startY; y <= endY; y++) {
      outputimage(startX,y) = pixColor;
      outputimage(endX,y) = pixColor;
   }

   // draw the top and bottom boundaries
   for (x = startX; x <= endX;  x++) {
      outputimage(x,startY) = pixColor;
      outputimage(x,endY) = pixColor;
   }

   // write the output to a JPEG file
   writeJpeg( outputimage, "images/output/box.jpg", 70 ); 
      // the last parameter is quality (1..100 = best)
}




