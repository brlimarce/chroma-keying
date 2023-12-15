
/* 
   Finding specific colors using the HSI color space
*/


#define IMAGE_RANGE_CHECK

#include "image.h"
#include "jpegio.h"
#include "color.h"


int main () {

   double pi = 3.14159;
   
   // the following defines a range of colors in the Hue-Saturation-Intensity color space
   double minimumHue = 0;   // range is 0.0 to 2 * pi
   double maximumHue = pi/12;   // range is 0.0 to 2 * pi
   double minimumSaturation = 0.3;   // range is 0.0 to 1.0
   double maximumSaturation = 0.4;   // range is 0.0 to 1.0

   int x,y, height,width, pixColor;
   double red, green, blue;
   double hue, saturation, intensity;
   RGBImage inputImage, outputImage;

   readJpeg( inputImage, "images/ex10/skin-color.jpg" );
   height = inputImage.height();
   width  = inputImage.width();

   outputImage.resize( width,height );  // set dimensions

   // for each pixel
   for (x = 0; x < width;  x++) {
   for (y = 0; y < height; y++) {
     pixColor = inputImage(x,y);
     red = RED(pixColor);
     green = GREEN(pixColor);
     blue = BLUE(pixColor);

     // convert from RGB to HSI 
     RGBtoHSI( red, green, blue, hue, saturation, intensity ); 

     // if the hue and saturation are within the search range
     // then retain the original pixel, otherwise it becomes white
     if (minimumHue <= hue && hue <= maximumHue &&
         minimumSaturation <= saturation && saturation <= maximumSaturation) {
        outputImage(x,y) = pixColor;   // retain the color
     }
     else   // color it white
        outputImage(x,y) = COLOR_RGB(255,255,255);        

   }
   }

   writeJpeg( outputImage, "images/output/hsi-pick.jpg", 70 ); 
}




