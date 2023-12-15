
/* 
   Changing the image brightness
    
*/


#define IMAGE_RANGE_CHECK

#include "image.h"
#include "jpegio.h"
#include "math.h"
#include "stdio.h"

int main () {

   int x,y, height,width;
   double red,green,blue;
   double redAdjusted, greenAdjusted, blueAdjusted;
   int pix;
   RGBImage inputimage, outputimage;

   // this is the brightness adjustment parameter
   // whose range is -1.0 (very dark) to 1.0 (very bright)
   //   > 0 will brighten
   //  <  0 will  darken
   double brightAdjust = 0.2;

   // read the JPEG file
   readJpeg( inputimage, "images/dark_scene.jpg" );
   height = inputimage.height();
   width  = inputimage.width();

   printf("\nBrightness adjustment parameter (brightAdjust) = %3.2f \n",brightAdjust);
   
   // set the dimensions of the output image
   outputimage.resize( width,height );  

   // adjust the brightness of the Red, Green and Blue components
   for (y = 0; y < height; y++) {
    for (x = 0; x < width;  x++) {
      pix = inputimage(x,y);
      red = RED(pix) / 255.0;     // scale the r,g,b from 0..255 to 0..1
      green = GREEN(pix) / 255.0;
      blue = BLUE(pix) / 255.0;

      if (brightAdjust > 0.0) {    // brighten the image
         redAdjusted = red * (1.0 - brightAdjust) + brightAdjust;
         greenAdjusted = green * (1.0 - brightAdjust) + brightAdjust;
         blueAdjusted = blue * (1.0 - brightAdjust) + brightAdjust;
      }
      else {    // darken the image
         redAdjusted = red * (1.0 - fabs(brightAdjust));
         greenAdjusted = green * (1.0 - fabs(brightAdjust));
         blueAdjusted = blue * (1.0 - fabs(brightAdjust));
      }
/*  color negative
      redAdjusted = 1.0 - red;
      greenAdjusted = 1.0 - green;
      blueAdjusted = 1.0 - blue;
*/

      // scale them back from 0..1 to 0..255
      outputimage(x,y) = COLOR_RGB((unsigned char)(redAdjusted*255), 
     (unsigned char)(greenAdjusted*255), (unsigned char)(blueAdjusted*255));
    }
   }
   // write the output to a JPEG file
   writeJpeg( outputimage, "images/output/bright1.jpg", 70 ); 
      // the last parameter is quality (1..100 = best)
}
