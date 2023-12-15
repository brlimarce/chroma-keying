
/* 
Read a series of JPEG images and do some processing

*/


#define IMAGE_RANGE_CHECK

#include "image.h"
#include "jpegio.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

int main () {

  char inputDir[200];
  char inputFileName[200];
  char outputFileName[200];
  int f;
  int frameStart = 23;
  int frameEnd = 246;
  RGBImage inputImageRGB, outputImageRGB;
  int x1, y1, x2, y2;
  strcpy(inputDir,"/media/blask-USB/10-05-18-image-chamber");

  for (f = frameStart; f <= frameEnd; f++) {
    
    sprintf( inputFileName, "%s/%03d.JPG", inputDir,f);  
    readJpeg( inputImageRGB, inputFileName );

    // process here
    outputImageRGB = inputImageRGB;

    sprintf( outputFileName, "%s/output/01-16-%03d.jpg", inputDir,f);  
    writeJpeg( outputImageRGB, outputFileName, 90);
  }
  return 0;
}
