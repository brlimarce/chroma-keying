
/* 
    Detecting the changes between two images
    
*/


#define IMAGE_RANGE_CHECK

#include "image.h"
#include "jpegio.h"


int main () {

   int threshold = 40;   

   int x,y, height,width;
   unsigned char gray;
   int pix1, pix2;
   int r1,g1,b1, r2,g2,b2;
   RGBImage inputImage1, inputImage2, outputImage;

   // read the JPEG files of the two input images
   // the two must have the same dimensions

   readJpeg( inputImage1, "images/scene1.jpg" );
   readJpeg( inputImage2, "images/change4.jpg" );
   height = inputImage1.height();
   width  = inputImage1.width();

   // set the dimensions of the output image
   outputImage.resize( width,height );  

   // determine which pixels are significantly different
   // between the two images
   for (y = 0; y < height; y++) {
     for (x = 0; x < width;  x++) {
       pix1 = inputImage1(x,y);
       pix2 = inputImage2(x,y);
       r1 = RED(pix1);  g1 = GREEN(pix1);  b1 = BLUE(pix1);
       r2 = RED(pix2);  g2 = GREEN(pix2);  b2 = BLUE(pix2);

       // decide if pix2 is significantly different from pix1
       // a good alternaative could be to use tthe euclidean distance between (r1,g1,b1) and (r2,g2,b2)
       if (abs(r1-r2) + abs(g1-g2) + abs(b1-b2) > threshold) 
         outputImage(x,y) = pix2;
       else
         outputImage(x,y) = COLOR_RGB(255,255,255);
     }
   }

   // write the output image to a JPEG file
   writeJpeg( outputImage, "images/output/changes.jpg", 70 ); 
}




