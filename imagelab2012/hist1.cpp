
/* 
    Draw the intensity histogram of an image
*/

#define IMAGE_RANGE_CHECK

#include "image.h"
#include "jpegio.h"
#include "tools.h"
#include "color.h"
#include "stdio.h"




int main (int argc, char *argv[]) {  
   int x,y, height,width;
   int g, gMax;
   RGBImage inputColorImage;
   // the following is an image whose pixel's type is unsigned char
   Image<unsigned char> grayImage;   // each pixel has type   unsigned char
       // unsigned char is  8 bits   and the range of values  is  0 "black" . . 255  "white"
   Image<int> hist;   // not really an image, but it uses Image<int> a table of numbers
   RGBImage histImage;
   
   // read the JPEG file
   readJpeg( inputColorImage, argv[1] );
   height = inputColorImage.height();
   width  = inputColorImage.width();
   printf("The image width is %d and height is %d \n",width,height);
   
   // make a gray image from the RGB image
   // input image is    inputColorImage
  // the output will be stored in    grayImage    which was declared earlier
  makeGrayFromRGB( grayImage, inputColorImage );
  
  // compute the intensity histogram
  hist.resize(256,1);
  hist.setAll(0);
  gMax = 0;
  for (x = 0; x < width; x++) {
    for (y = 0; y < height; y++) {
      g = grayImage(x,y);
      hist(g,0)++;
      if (hist(g,0) > hist(gMax,0))
        gMax = g;
    }
  }
  // draw the histogram onto the image histImage
  histImage.resize(256,256);
  histImage.setAll(COLOR_RGB(255,255,255));   // set all pixels to "white"
  for (x = 0; x < 256; x++)
    for (y = 0; y < min(255*hist(x,0)/hist(gMax,0),255); y++)  // the frequencies are scaled down so the bar will fit in the image
      histImage(x,255-y) = COLOR_RGB(0,0,0);

  writeJpeg( histImage, argv[2], 90 );  // write as a JPEG file
}

