/* 
   Simple background subtraction
*/

#define IMAGE_RANGE_CHECK
#define MAX_NUM_FRAMES 50

#include "image.h"
#include "jpegio.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

int compare (const void * a, const void * b)
{
  return ( *(int*)a - *(int*)b );
}

int main () {
  char inputFileFormat[100];
  char outputFileFormat[100];
  char imageFileName[100];
  int numFrames;  
  int numLearnFrames;
  int frameStart;
  int frameInterval;

  int width,height,f,x,y,pix;
  RGBImage inputFrame, outputFrame;
  Image<unsigned char> grayFrame[MAX_NUM_FRAMES];
  Image<unsigned char> background, gray1;
  Image<unsigned char> binary1, binary2;
  int list[MAX_NUM_FRAMES];
  int foregroundDifferenceThreshold;
  
  system("mkdir images/backsub");
  strcpy(inputFileFormat,"images/student/student%04d.jpg");
  strcpy(outputFileFormat,"images/backsub/backsub%04d.jpg");

  numFrames = 200;   // total number of frames in the sequence
  
  // "learn" the background from these frames
  frameStart = 10;  // start from tthis frame
  frameInterval = 10;  // skip every frameInterval
  numLearnFrames = 28;  // total number of "learning" frames
  foregroundDifferenceThreshold = 25;

  // fetch the frames from which the background will be learned
  for (f = 0; f < numLearnFrames; f++) {
    sprintf( imageFileName, inputFileFormat, frameStart + f*frameInterval);
    readJpeg( inputFrame, imageFileName );
    width = inputFrame.width();
    height = inputFrame.height();
    grayFrame[f].resize(width,height);
    // convert each color image to gray scale
    for (x = 0; x < width; x++) {
      for (y = 0; y < height; y++) {
        pix = inputFrame(x,y);
        grayFrame[f](x,y) = (RED(pix) + GREEN(pix) + BLUE(pix))/3;
      }
    }
  }
  
  // compute the background gray image
  background.resize(width,height);
  outputFrame.resize(width,height);
  for (x = 0; x < width; x++) {    // for every pixel  (x,y)
    for (y = 0; y < height; y++) {
      for (f = 0; f < numLearnFrames; f++) {
        list[f] = grayFrame[f](x,y);  // put every frame's (x,y) value in a list
      }
      qsort (list, numLearnFrames, sizeof(int), compare);
      background(x,y) = list[numLearnFrames / 2];   // get median value
      outputFrame(x,y) = COLOR_RGB(background(x,y),background(x,y),background(x,y));
    }  
  }
  sprintf( imageFileName, outputFileFormat, -1);
  writeJpeg( outputFrame, imageFileName, 85 );   // save the background image
  
  // subtract each image from the background
  for (f = 0; f < numFrames; f++) {
    sprintf( imageFileName, inputFileFormat, f);
    readJpeg( inputFrame, imageFileName );
    width = inputFrame.width();
    height = inputFrame.height();
    gray1.resize(width,height);
    binary1.resize(width,height);
    // convert each color image to gray scale
    for (x = 0; x < width; x++) {
      for (y = 0; y < height; y++) {
        pix = inputFrame(x,y);
        gray1(x,y) = (RED(pix) + GREEN(pix) + BLUE(pix))/3;  // gray scale
        if (abs(gray1(x,y)-background(x,y)) > foregroundDifferenceThreshold)
          binary1(x,y) = 1;   // foreground
        else
          binary1(x,y) = 0;   // background
      }
    }
   
    // output of backgroound subtraction (binary1) can be cleaned here
    // binary2  will be the output of cleaning  binary1
    binary2 = binary1;
    
    // write the output image as  input image beside the binary image (result of background subtraction)
    outputFrame.resize(width*2,height);
    for (x = 0; x < width; x++) {
      for (y = 0; y < height; y++) {
        outputFrame(x,y) = inputFrame(x,y);  // left side contains the original image
        if (binary2(x,y) == 1)
          outputFrame(x+width,y) = COLOR_RGB(0,0,0);   
        else
          outputFrame(x+width,y) = COLOR_RGB(255,255,255);  
      }
    }
    sprintf( imageFileName, outputFileFormat, f);
    writeJpeg( outputFrame, imageFileName, 85 );
  }
}
