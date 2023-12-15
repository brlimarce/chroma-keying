
/* 
    Converting a color image to black-and-white
*/

#define IMAGE_RANGE_CHECK

#include "image.h"
#include "jpegio.h"
#include "tools.h"
#include "stdio.h"
#include "color.h"


int main () {
   int x,y, height,width;
   RGBImage inputColorImage, outputColorImage;
   Image<unsigned char> grayImage;   // each pixel has type   unsigned char
       // unsigned char is  8 bits   and the range of values  is  0 "black" . . 255  "white"
   unsigned char grayValue;
   
   grayImage.resize( 320,240 );  // allocate memory for the image
       // set the image's dimensions to  320 pixels wide  and  240 pixels high
   height = grayImage.height();   // the image's height
   width = grayImage.width();   // get the image's width in pixels
	   
   // change a pixel value
   grayImage(50,75)  = 210;   // pixel location (50,75) now has a value of 210
   // change the values of a "sub-image"
   for (x = 25; x < 68; x++)
     for (y = 40; y < 200; y++)  
  	   // assign the value 39 to pixel location (x,y) in the image object grayImage
       grayImage(x,y) = 39;  
   
   // get the value of a pixel
   unsigned char value2;
   value2 = grayImage(34,150);
   
   Image<unsigned char> grayImage2, grayImage3;
   grayImage2 = grayImage;   // make a copy of the image
   
   // read the JPEG file
   readJpeg( inputColorImage, "images/kristen.jpg" );
   height = inputColorImage.height();
   width  = inputColorImage.width();
   printf("The image width is %d and height is %d \n",width,height);
   
   // make a gray image from the RGB image
   // input image is    inputColorImage
   // the output will be stored in    grayImage    which was declared earlier
   makeGrayFromRGB( grayImage, inputColorImage );
   
   // set the dimensions of the output image
   outputColorImage.resize( width,height );  

   // make an RGB version of the gray image
  for (y = 0; y < height; y++) {
    for (x = 0; x < width;  x++) {
      // fetch the value from the gray image
      grayValue = grayImage(x,y);
      // make an RGB color from the gray value
      outputColorImage(x,y) = COLOR_RGB(grayValue,grayValue,grayValue);
    }
  }

  // restore the color of a "strip"
  for (y = 50; y < 125; y++) {   
    for (x = 0; x < width;  x++) {
      // restore the color of pixel  (x,y)
      outputColorImage(x,y) = inputColorImage(x,y);
    }
  }

  // write the output image to a JPEG file
  // the last parameter is JPEG compression quality (1 = poor . . 100 = best)
  writeJpeg( outputColorImage, "images/output/gray.jpg", 90 ); 
  
  RGBImage outputColorImage2;
  
  outputColorImage2 = inputColorImage;   // copy kristen
  //draw an orange diagonal line
  for (x = 5; x < 200; x++)
    outputColorImage2(x,x) = COLOR_RGB(255,128,0);

  // halve the r,g,b of each pixel within a region
  unsigned char r,g,b;
  int pix;
  for (x = 120; x < 220; x++) {
    for (y = 30; y < 90; y++) {
      pix = inputColorImage(x,y);
      r = RED(pix);
      g = GREEN(pix);
      b = BLUE(pix);
      outputColorImage2(x,y) = COLOR_RGB(r/2,g/2,b/2);
    }
  }
  
  writeJpeg( outputColorImage2, "images/output/diagonal.jpg", 90 ); // the higher the third param, the smaller the file, lesser detail
}
