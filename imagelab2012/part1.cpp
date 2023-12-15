
/* 
  Converting a rectangular part of a color image to gray scale
    
*/


#define IMAGE_RANGE_CHECK

#include "image.h"
#include "jpegio.h"


int main () {
   // bounds of the region that will become "gray"
   int startX = 120;    
   int startY = 25;
   int endX = 275;
   int endY = 150;

   int x,y, height,width;
   unsigned char gray;
   int pix;

   RGBImage inputimage, outputimage;

   // read the JPEG file
   readJpeg( inputimage, "images/kristen.jpg" );
   height = inputimage.height();
   width  = inputimage.width();

   // set the dimensions of the output image
   outputimage.resize( width,height );  

   // compute the gray scale of part the colored picture
   for (y = 0; y < height; y++)
   for (x = 0; x < width;  x++) {
      pix = inputimage(x,y);
      if (startY <= y && y <= endY &&
          startX <= x && x <= endX) {
        gray = (RED(pix) + GREEN(pix) + BLUE(pix)) / 3;
        outputimage(x,y) = COLOR_RGB(gray,gray,gray);
      } 
      else {
        outputimage(x,y) = pix;
      }
   }

   // write the output to a JPEG file
   writeJpeg( outputimage, "images/output/partgray.jpg", 70 ); 
      // the last parameter is quality (1..100 = best)
}




