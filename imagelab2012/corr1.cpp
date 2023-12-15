
/* 
   A simple correlation-based object detector.
   Detect the occlusion of region of interest (ROI) on the image.
   The ROI is constantly compared with that of the computed background image.    
*/


#define IMAGE_RANGE_CHECK

#include "image.h"
#include "jpegio.h"
#include "color.h"
#include "stdio.h"
#include "string.h"
#include <algo.h>
#include "filter.h"

// this is a function that takes an input RGBImage and makes a gray image
void makeGrayFromRGB( Image<unsigned char> & grayOutput, 
                         const RGBImage & colorImage )  {
  int width,height,x,y;
  int pix;
  unsigned char red, green, blue;
  
  width = colorImage.width();
  height = colorImage.height();
  grayOutput.resize( width,height );  // output image has same dimensions as the input color image

  //  visit every pixel in the input image  
  for (x = 0; x < width; x++) {
    for (y = 0; y < height; y++) {
      //  pix is of type  int
      pix = colorImage(x,y);   // fetch the colored pixel from colorImage(x,y)
      red = RED(pix);       // get the RED value
      green = GREEN(pix);   // get the GREEN value
      blue = BLUE(pix);     // get the BLUE value
      // compute the  "black-and-white"  or  "gray value"  or "intensity"  of the pixel
      grayOutput(x,y) = (red + green + blue) / 3;      // average of the red, green and blue
    }  
  }
}    // end of function 


double correlationROI( const Image<unsigned char> & backgroundPattern, const Image<unsigned char> & inputGray, int x1, int y1, int x2, int y2) {
  int m,n;
  int valueA, valueB;
  double sumA, sumA2, sumB, sumB2, sumAB;
  int area = (x2 - x1) * (y2 - y1);
  double corr;
  sumA = sumA2 = sumB = sumB2 = sumAB = 0;
  for (m = x1; m < x2; m++)  {
    for (n = y1; n < y2; n++) {
      valueA = inputGray(m,n);
      valueB = backgroundPattern(m,n);
      sumA += valueA;
      sumB += valueB;
      sumA2 += valueA * valueA;
      sumB2 += valueB * valueB;
      sumAB += valueA * valueB;
    }
  }
  corr = (sumAB - sumA*sumB/area)/area 
         / sqrt((sumA2 - sumA*sumA/area)/area * (sumB2 - sumB*sumB/area)/area);
  return corr;
}


void displayValueHistory( RGBImage & outFrame, Image<double> & history, double newValue, double threshold, double maxValue, int windowHeight, int winX, int winY) {
  int i,x,y,ybar;
  int windowWidth = history.width();
  for (i = 0; i < windowWidth - 1; i++)
    history[i] = history[i+1];
  history[windowWidth-1] = fabs(newValue);
  for (x = 0; x < windowWidth; x++)
    for (y = 0; y < windowHeight; y++)
      outFrame(winX + x, winY + y) = COLOR_RGB(0,0,0);
  for (x = 0; x < windowWidth; x++) {
    ybar = (int) (windowHeight * history[x] / maxValue);
    for (y = 0; y < ybar; y++)
      outFrame(winX + x, winY + windowHeight - y) = COLOR_RGB(255,128,0);
    outFrame(winX + x, winY + windowHeight - windowHeight * threshold / maxValue) = COLOR_RGB(255,0,0);
  }
}


int compare (const void * a, const void * b)
{
  return ( *(int*)a - *(int*)b );
}


int main () {

  char inputFileFormat[100];
  char outputFileFormat[100];
  int frameStart;
  int frameEnd;
  // int x1 = 145, y1 = 40, x2 = 189, y2 =  67;
  int x1 = 131, y1 = 158, x2 = 164, y2 = 182;
  strcpy(inputFileFormat,"images/sixlanes1/cars%04d.jpg");
  system("mkdir images/output/sixlanes1-out");
  strcpy(outputFileFormat,"images/output/sixlanes1-out/%04d.jpg");
  frameStart = 0;
  frameEnd = 900;

  int numFrames = frameEnd - frameStart + 1;
  int x,y,f,m,n,width,height;

  int interval = 5;
  int numGray = 20;
  double corrThreshold = 0.5;
  
  Image<unsigned char> gray, gray2, grayA[numGray], grayMedian;
  Image<unsigned char> binary1, binary2;
  int grayList[numGray];
  int g;
  
  double v, corr;
  int npix,p;
  Image<double> corrHistory;
  corrHistory.resize(60,1);    
  corrHistory.setAll(0.0);

  RGBImage currentFrame,prevFrame,outFrame;
  char imageFileName[100];

  for (f = 0; f < numGray; f++) {
    sprintf( imageFileName, inputFileFormat, frameStart + f * interval);  
    readJpeg( currentFrame, imageFileName );
    makeGrayFromRGB( grayA[f], currentFrame );
  }

  width = currentFrame.width();
  height = currentFrame.height();
  npix = width * height;
  
  gray.resize(width,height);
  grayMedian.resize(width,height);
  binary1.resize(width,height);

  printf("Computing background image...\n");
  printf("Sorting... \n");
  for (p = 0; p < npix; p++) {
    for (g = 0; g < numGray; g++) {
      grayList[g] = grayA[g][p];
    }
    qsort (grayList, numGray, sizeof(int), compare);
    // orderStatFilter_QUICKSORT( grayList, 0, numGray );
    grayMedian[p] = grayList[numGray / 2];
  }
  
  for (f = 0; f < numFrames; f++)  {
    prevFrame = currentFrame;
    sprintf( imageFileName, inputFileFormat, f + frameStart);
    readJpeg( currentFrame, imageFileName );
    gray2 = gray;
    makeGrayFromRGB( gray, currentFrame );
    outFrame = currentFrame;

    
    printf("Computing correlation for frame %d \n", f);
    corr = correlationROI( grayMedian, gray, x1,y1,x2,y2 );
    // display a 50-high history window at (10,180)
    displayValueHistory( outFrame, corrHistory, corr, corrThreshold, 1.1, 50, 10, 180);  
         
    for (x = x1; x < x2; x++)  // draw a green box
      for (y = y1; y < y2; y++)
        if (x == x1 || x == x2-1 || y == y1 || y == y2-1)
          outFrame(x,y) = (corr > corrThreshold) ? COLOR_RGB(0,255,0):COLOR_RGB(255,0,0);

    sprintf( imageFileName, outputFileFormat, f + frameStart);
    writeJpeg( outFrame, imageFileName, 80 );
  }  
}
