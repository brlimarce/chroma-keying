
/* 
  Modifying Hue, Saturation and Intensity components
*/


#define IMAGE_RANGE_CHECK

#include "image.h"
#include "jpegio.h"
#include "color.h"


int main () {
   int x,y, height,width, pixColor;
   double red, green, blue;
   double hue, saturation, intensity, satNew, intNew;
   RGBImage inputImage, outputImage;

   double saturationAdjust = 0.0;   // adjustment range is -1.0 to 1.0
   double intensityAdjust = 0.0; // -0.25;   adjustment range is -1.0 to 1.0

   readJpeg( inputImage, "images/kristen.jpg" );
   height = inputImage.height();
   width  = inputImage.width();

   outputImage.resize( width,height );  // set dimensions

   for (x = 0; x < width;  x++) {
     for (y = 0; y < height; y++) {
       pixColor = inputImage(x,y);
       // get the red, green, and blue components
       red = RED(pixColor);
       green = GREEN(pixColor);
       blue = BLUE(pixColor);

       // convert from RGB to HSI 
       RGBtoHSI( red, green, blue, hue, saturation, intensity ); 

       // modify the saturation and intensity
       if (saturationAdjust > 0.0)    // increase saturation
          satNew = saturation * (1.0 - saturationAdjust) + saturationAdjust;
       else     // decrease the saturation
          satNew = saturation * (1.0 - fabs(saturationAdjust));

       if (intensityAdjust > 0.0)    // increase intensity
         intNew = intensity * (1.0 - intensityAdjust) + intensityAdjust;
       else     // decrease the intensity
         intNew = intensity * (1.0 - fabs(saturationAdjust));

       // hue += 3.14/2;

       // convert HSI back to RGB
       HSItoRGB( hue, satNew, intNew, red, green, blue );

       outputImage.setPix( x,y, red, green, blue);
     }
   }

   writeJpeg( outputImage, "images/output/hsi-adjust.jpg", 70 ); 
}




