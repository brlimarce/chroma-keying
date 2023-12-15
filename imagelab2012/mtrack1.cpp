
/* 
    Tracking a small, moving object using motion tracking  
    
*/


#define IMAGE_RANGE_CHECK

#include "image.h"
#include "jpegio.h"
#include "color.h"
#include "stdio.h"
#include "string.h"
#include "tools.h"  // <algo.h>

int main () {

  char inputFileFormat[100];
  char outputFileFormat[100];
  int frameStart;
  int frameEnd;
  // initialize object location in first frame
  int objectStartX;
  int objectStartY;
  int objectEndX;
  int objectEndY;
  int followCar = 0;
  
  if (followCar) {
    strcpy(inputFileFormat,"images/car1/%04d.jpeg");
    frameStart = 0;
    frameEnd = 242;
    // initialize object location in first frame
    objectStartX = 170;
    objectStartY = 30;
    objectEndX = 260;
    objectEndY = 120;
  }

  if (!followCar) {
    strcpy(inputFileFormat,"images/kids1/%04d.jpeg");
    frameStart = 0;
    frameEnd = 292;
    // initialize object location in first frame
    objectStartX = 36;
    objectStartY = 112;
    objectEndX = 60;
    objectEndY = 147;
  }
  
  system("mkdir images/motiontrack");
  strcpy(outputFileFormat,"images/motiontrack/%04d.jpg");
    
  int numFrames = frameEnd - frameStart + 1;
  int f,m,n,x,y,xt,yt,xtp,ytp,width,height;
  int vicinityMargin = 10;  // radius of S; or half the width of S
  int vicHalfWidth,vicHalfHeight;  // vicinity
  int pix, pix1, pix2, pix3;
  
  Image<int> trajectory(numFrames,2),motion;
  int weight,sumWeightedX,sumWeightedY,sumWeights;
  
  RGBImage currentFrame,prevFrame,prevFrame2,
           prevFrame3,outFrame;
  char imageFileName[100];

  vicHalfWidth = (objectEndX - objectStartX + vicinityMargin) / 2;
  vicHalfHeight = (objectEndY - objectStartY + vicinityMargin) / 2;
  trajectory(0,0) = (objectEndX + objectStartX) / 2;
  trajectory(0,1) = (objectEndY + objectStartY) / 2;
  trajectory(1,0) = (objectEndX + objectStartX) / 2;
  trajectory(1,1) = (objectEndY + objectStartY) / 2;
  trajectory(2,0) = (objectEndX + objectStartX) / 2;
  trajectory(2,1) = (objectEndY + objectStartY) / 2;
  
  sprintf( imageFileName, inputFileFormat,frameStart);
  readJpeg( currentFrame, imageFileName );
  width = currentFrame.width();
  height = currentFrame.height();
  prevFrame = currentFrame;
  prevFrame2 = prevFrame;
  prevFrame3 = prevFrame2;
        
// find the object in the next frame
  for (f = 0; f < numFrames; f++)  {
    prevFrame3 = prevFrame2;
    prevFrame2 = prevFrame;
    prevFrame = currentFrame;
    sprintf( imageFileName, inputFileFormat, f + frameStart);
    readJpeg( currentFrame, imageFileName );
    
    if (f > 2) {
      outFrame.resize(width,height*2);
      outFrame.setAll(COLOR_RGB(0,0,0));
    
      motion.resize(width,height);   // "difference" image
      for (x = 0; x < width; x++)
        for (y = 0; y < height; y++) {
          pix = currentFrame(x,y);
          pix1 = prevFrame(x,y);
          pix2 = prevFrame2(x,y);
          pix3 = prevFrame3(x,y);
          // RGBtoHSI(RED(pix),GREEN(pix),BLUE(pix),hue,sat,inten);
          motion(x,y) = weight = abs(RED(pix)-RED(pix3)) + 
          abs(GREEN(pix)-GREEN(pix3)) + abs(BLUE(pix)-BLUE(pix3));
  
          outFrame(x,y) = currentFrame(x,y);
          weight = min(weight,255);
          outFrame(x,y+height) = COLOR_RGB(weight,weight,weight);
        }   

        // object-tracking on the motion image
        xt = trajectory(f-1,0);
        yt = trajectory(f-1,1);
        sumWeightedX = sumWeightedY = sumWeights = 0;    
        for (m = -vicHalfWidth; m < vicHalfWidth; m++) {
          for (n = -vicHalfHeight; n < vicHalfHeight; n++) {
            if (xt+m >= 0 && xt+m < width &&  // if within the image bounds
                yt+n >= 0 && yt+n < height)  {
              weight = motion(xt+m,yt+n);
              sumWeightedX += m * weight;
              sumWeightedY += n * weight;
              sumWeights += weight; 
            }
          }
        }
  
        // find the center of the weights in the search area
        xtp = trajectory(f,0) = sumWeightedX / (sumWeights+1) + xt;
        ytp = trajectory(f,1) = sumWeightedY / (sumWeights+1) + yt;
        printf("   xtp = %d, ytp = %d \n",xtp,ytp);
    
        // put an orange box on the object
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
    }
  
    for (f = 0; f < numFrames; f++) {
      currentFrame(trajectory(f,0),trajectory(f,1)) = COLOR_RGB(255,128,0);
      currentFrame(trajectory(f,0)+1,trajectory(f,1)) = COLOR_RGB(255,128,0);
      currentFrame(trajectory(f,0),trajectory(f,1)+1) = COLOR_RGB(255,128,0);
      currentFrame(trajectory(f,0)+1,trajectory(f,1)+1) = COLOR_RGB(255,128,0);
    }
    writeJpeg( currentFrame, "images/output/traject.jpg", 90 );
}

