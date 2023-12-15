
/* 
   Psuedo-coloring a gray scale image
    
*/


#define IMAGE_RANGE_CHECK

#include "image.h"
#include "jpegio.h"


int main () {
   int x,y, height,width;
   double gray;
   int pix, color;
   RGBImage inputimage, outputimage;

   // read the JPEG file
   readJpeg( inputimage, "images/star1.jpg" );
   height = inputimage.height();
   width  = inputimage.width();

   // set the dimensions of the output image
   outputimage.resize( width,height );  

   // compute the gray scale of the colored picture
   // and assign a color to each range of the gray scale
   for (y = 0; y < height; y++)
   for (x = 0; x < width;  x++) {
      pix = inputimage(x,y);
      gray = (RED(pix) + GREEN(pix) + BLUE(pix)) / 3.0;
      gray = gray / 255.0;   // scale it to 0..1
      if (gray > 0.95)  color = COLOR_RGB(255,255,255);   // white
      else if (gray > 0.90)  color = COLOR_RGB(255,128,128);   // light red
      else if (gray > 0.85)  color = COLOR_RGB(255,0,0);   // red
      else if (gray > 0.80)  color = COLOR_RGB(255,128,0);   // orange
      else if (gray > 0.75)  color = COLOR_RGB(0,255,0);   // green
      else if (gray > 0.70)  color = COLOR_RGB(0,128,128);   // 
      else if (gray > 0.65)  color = COLOR_RGB(0,0,255);   // blue
      else if (gray > 0.60)  color = COLOR_RGB(0,0,128);   // dark blue
      else color = COLOR_RGB((int)(gray*255),(int)(gray*255),(int)(gray*255));  // gray

      outputimage(x,y) = color;
   }

   // write the output pseudo-color image to a JPEG file
   writeJpeg( outputimage, "images/output/pseudo.jpg", 70 ); 
}




