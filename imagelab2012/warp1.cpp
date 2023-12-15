
/* 
    Warping an image using simple 2-D transformations:   Translation, Rotation and Scaling
*/

#define IMAGE_RANGE_CHECK

#include "image.h"
#include "jpegio.h"
#include "tools.h"
#include "stdio.h"
#include "stdlib.h"
  
int main () {
  int scalePercent = 200;
  double angleDegrees = 30;
  double angleRadians = angleDegrees * 3.14159 / 180.0;
  int translateX = 30;
  int translateY = 50;
  
  int x,y, height,width, frame;
  RGBImage img1, img2, img3, img4;
  char fileName[200];
  
  readJpeg( img1, "images/kristen.jpg" );
  height = img1.height();   // get the image's height in pixels
  width = img1.width();   // get the image's width in pixels

  system("mkdir images/output/rotate");
  for (int frame = 0; frame < 100; frame++)  {
    scaleImageRGB( img2, img1, width * (scalePercent-frame*1.25) / 100, height * (scalePercent-frame*1.25) / 100);
    translateRGB( img3, img2, translateX,translateY);
    // rotate the image by an angle angleRadians, around a specified center of rotation
    rotateRGB( img4, img3, angleRadians + frame*0.05, img2.width() / 2, img2.height() / 2 );  // rotate around the center

    sprintf( fileName, "images/output/rotate/warp-%03d.jpg", frame);
    writeJpeg( img3, fileName, 100 );
  }    
}
