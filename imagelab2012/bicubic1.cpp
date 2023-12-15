
/* 
   Scale a color image using Bicubic interpolation  
*/


#define IMAGE_RANGE_CHECK

#include "image.h"
#include "jpegio.h"
#include <math.h>


double cube( double x )  {
  return (x*x*x);
}

double clip( double x )  {
  return ((x > 0.0) ? x : 0.0);
}

double bicubicWeighingFunction( double x )  {
  return (
    1.0/6.0 * 
    ( cube(clip(x+2)) - 4 * cube(clip(x+1))
      + 6 * cube(clip(x)) - 4 * cube(clip(x-1))
    )
  );
}


int main () {
   int xp,yp, height,width;
   RGBImage inputimage, outputimage;
   double x,y,sumRed16,sumGreen16,sumBlue16,dx,dy,weightFactor;
   int m,n,i,j;

   int newHeight = 700;
   int newWidth = 300;
      
   // read the JPEG file
   readJpeg( inputimage, (char *) "images/kristen.jpg" );
   height = inputimage.height();
   width  = inputimage.width();

   // set the dimensions of the output image
   outputimage.resize( newWidth,newHeight );  

   // compute each pixel in the interpolated image
   // ISSUE:  How do we determine the boundaries of the loop?
   for (yp = 3; yp < newHeight-7; yp++) {
   for (xp = 3; xp < newWidth-7;  xp++) { 
     x = 1.0 * xp * width / newWidth;
     y = 1.0 * yp * height / newHeight;
     i = (int) x;
     j = (int) y;
     dx = x - i;
     dy = y - j;
     sumRed16 = sumGreen16 = sumBlue16 = 0.0;
     for (m = -1; m < 3; m++) {
     for (n = -1; n < 3; n++) {
       weightFactor = bicubicWeighingFunction(m - dx) 
                      * bicubicWeighingFunction(dy - n);
       sumRed16 += RED(inputimage(i+m,j+n)) * weightFactor;
       sumGreen16 += GREEN(inputimage(i+m,j+n)) * weightFactor;
       sumBlue16 += BLUE(inputimage(i+m,j+n)) * weightFactor;
     }
     }
     outputimage(xp, yp) = COLOR_RGB((int) sumRed16,(int) 
                           sumGreen16,(int) sumBlue16);
   }
   }
       
   // write the output to a JPEG file
   system("mkdir images/output");
   writeJpeg( outputimage, (char *) "images/output/scaled_bicubic.jpg", 90 ); 
}




