
/* 
    Tracking a colored object using Histogram Backprojection  
    
*/


#define IMAGE_RANGE_CHECK

#include "image.h"
#include "jpegio.h"
#include "color.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

int main () {

  // trackBlue = 1 --> track the blue toy
  // trackBlue = 0 --> track the red ball
  int trackBlue = 1;  
 
  char inputFileFormat[100];
  char outputFileFormat[100];
  int frameStart;
  int frameEnd;
  // initialize object location in first frame
  int objectStartX;
  int objectStartY;
  int objectEndX;
  int objectEndY;

  if (trackBlue) {
    // blue toy in the two-object image sequence
    strcpy(inputFileFormat,"images/redblue/redblue%04d.jpg");
    frameStart = 0;
    frameEnd = 708;
    // initialize object location in first frame
    objectStartX = 137;
    objectStartY = 37;
    objectEndX = 176;
    objectEndY = 77;
  }
  else {
    // red ball in the two-object image sequence
    strcpy(inputFileFormat,"images/redblue/redblue%04d.jpg");
    frameStart = 0;
    frameEnd = 708;
    // initialize object location in first frame
    objectStartX = 250;
    objectStartY = 156;
    objectEndX = 302;
    objectEndY = 204;
  }
  
  system("mkdir images/colortrack");
  strcpy(outputFileFormat,"images/colortrack/%04d.jpg");
    
  int numFrames = frameEnd - frameStart + 1;
  int f,m,n,x,y,xt,yt,xtp,ytp,width,height;
  int vicinityMargin = 20;
  int vicHalfWidth,vicHalfHeight;  // vicinity
  int numHueBins = 10;
  int numSatBins = 10;
  int pix;
  double hue,sat,inten;
  double dHue = 2.0 * 3.14159 / numHueBins;
  double dSat = 1.0 / numSatBins;
  
  Image<int> trajectory(numFrames,2);
  Image<int> objectHistogram(numHueBins,numSatBins+1);
  Image<int> weightImage;
  int weight,sumWeightedX,sumWeightedY,sumWeights;
  
  RGBImage currentFrame,prevFrame,outFrame;
  char imageFileName[100];

  vicHalfWidth = (objectEndX - objectStartX + vicinityMargin) / 2;
  vicHalfHeight = (objectEndY - objectStartY + vicinityMargin) / 2;
  trajectory(0,0) = (objectEndX + objectStartX) / 2;
  trajectory(0,1) = (objectEndY + objectStartY) / 2;
  
  sprintf( imageFileName, inputFileFormat,frameStart);
  readJpeg( currentFrame, imageFileName );
  width = currentFrame.width();
  height = currentFrame.height();
  
// compute the histogram of the object  
  objectHistogram.setAll(0);
  for (x = objectStartX; x < objectEndX; x++) {
    for (y = objectStartY; y < objectEndY; y++) {
      pix = currentFrame(x,y);
      RGBtoHSI(RED(pix),GREEN(pix),BLUE(pix),hue,sat,inten);
      objectHistogram((int)(hue/dHue), (int)(sat/dSat))++;    // increment bin
    }
  }
  
// find the object in the next frame
  for (f = 1; f < numFrames; f++)  {
    prevFrame = currentFrame;
    sprintf( imageFileName, inputFileFormat, f + frameStart);
    readJpeg( currentFrame, imageFileName );
    
    xt = trajectory(f-1,0);
    yt = trajectory(f-1,1);
    sumWeightedX = sumWeightedY = sumWeights = 0;    
    for (m = -vicHalfWidth; m < vicHalfWidth; m++) {
      for (n = -vicHalfHeight; n < vicHalfHeight; n++) {
        // get the color of pixel in search area and
        // find its histogram weight
        if (xt+m >= 0 && xt+m < width &&  // if within the image bounds
        yt+n >= 0 && yt+n < height)  {
        pix = currentFrame(xt+m,yt+n);
        RGBtoHSI(RED(pix),GREEN(pix),BLUE(pix),hue,sat,inten);
        weight = objectHistogram((int)(hue/dHue), (int)(sat/dSat));
        sumWeightedX += m * weight;
        sumWeightedY += n * weight;
        sumWeights += weight; 
      }
    }
  }
  
  // find the center of the weights in the search area
  xtp = trajectory(f,0) = sumWeightedX / sumWeights + xt;
  ytp = trajectory(f,1) = sumWeightedY / sumWeights + yt;
  printf("   xtp = %d, ytp = %d \n",xtp,ytp);
    
  // put an orange box on the object
  outFrame = currentFrame;
  for (y = ytp-vicHalfHeight; y < ytp+vicHalfHeight; y++) {
    if (y >= 0 && y < height && xtp-vicHalfWidth >= 0 && xtp+vicHalfWidth < width) {
      outFrame(xtp-vicHalfWidth,y) = COLOR_RGB(255,128,0);
      outFrame(xtp-vicHalfWidth+1,y) = COLOR_RGB(255,128,0);
      outFrame(xtp+vicHalfWidth,y) = COLOR_RGB(255,128,0);
      outFrame(xtp+vicHalfWidth-1,y) = COLOR_RGB(255,128,0);
    }
  }
  for (x = xtp-vicHalfWidth; x < xtp+vicHalfWidth; x++) {
    if (x >= 0 && x < width && ytp-vicHalfHeight >= 0 && ytp+vicHalfHeight < height) {
      outFrame(x,ytp-vicHalfHeight) = COLOR_RGB(255,128,0);
      outFrame(x,ytp-vicHalfHeight+1) = COLOR_RGB(255,128,0);
      outFrame(x,ytp+vicHalfHeight) = COLOR_RGB(255,128,0);
      outFrame(x,ytp+vicHalfHeight-1) = COLOR_RGB(255,128,0);
    }
  }

  sprintf( imageFileName, outputFileFormat, f + frameStart);
  writeJpeg( outFrame, imageFileName, 65 );
  }
  
  for (f = 0; f < numFrames; f++) {
    currentFrame(trajectory(f,0),trajectory(f,1)) = COLOR_RGB(255,128,0);
  }
  writeJpeg( currentFrame, "images/output/traject.jpg", 90 );
}

