
/* 
  forms.c  and  forms.h
  31 Aug 2011
  this is the code needed by forms15.cpp
    
*/

#define IMAGE_RANGE_CHECK
#define DEBUG2 1

#define DEBUG1 0
#define IMAGE_DEBUG_DIRECTORY "/media/DATA-394GB/ovci-set/DEBUG/"
// /SET_OUTPUT/IMAGE_DEBUG/"

// in seeking the left and the right line, a moving window (width = SEEK_X_WINDOW) finds it
#define SEEK_X_WINDOW 30   

#include "forms.h"
#include "math.h"
// #include<algo.h>
#include "jpegio.h"
#include "tools.h"


void drawBox( RGBImage & img, int color, int thick, int x1, int y1, int x2, int y2) {
   int t,x,y;
   int w = img.width();
   int h = img.height();
   x1 = max(0,min(x1,w-1));              // make sure the box will be inside the image
   x2 = max(0,min(x2,w-1));
   y1 = max(0,min(y1,h-1));
   y2 = max(0,min(y2,h-1));
   
   for (t = 0; t < thick; t++) {
     for (y = y1; y < y2; y++) {     // draw the left and right boundaries
       img(x1+t,y) = color;
       img(x2-t,y) = color;
     }
     for (x = x1; x < x2; x++) {    // draw the top and bottom boundaries
       img(x,y1+t) = color;
       img(x,y2-t) = color;
     }
   }
}


void writeGrayJpeg(char* fileNamePrefix, int imageNumber, char* fileNameTag, int iterNumber, const Image<unsigned char> & grayInputImage) {
  int width = grayInputImage.width();
  int height = grayInputImage.height();
  int npix = width * height;
  int p;
  unsigned char gray;
  RGBImage outputRGB;
  char fileName[100];
  outputRGB.resize(width,height);
  for (p = 0; p < npix; p++) {
    gray = grayInputImage[p];
    outputRGB[p] = COLOR_RGB(gray,gray,gray);
  }
  sprintf(fileName,"%s%04d%s%02d.jpg", fileNamePrefix, imageNumber, fileNameTag, iterNumber);
  writeJpeg( outputRGB, fileName, 90 );
}

void centerOfMass( const Image<unsigned char> & img, int startX, int startY, int endX, int endY, int limitX1, int limitY1, int limitX2, int limitY2, int & centerX, int & centerY)  {

  // the center is restricted to the rectangle limits
  startX = max(limitX1,min(startX,limitX2));
  startY = max(limitY1,min(startY,limitY2));
  endX = max(limitX1,min(endX,limitX2));
  endY = max(limitY1,min(endY,limitY2));
  /*
  if (0) {   // median-based centering
    for (y = startY; y < endY; y++) {
      for (x = startX; x < endX; x++) {
        if ((weight = 255 - img(x,y)) >= 0) {  // invert the gray image first and don't include the dark pixels!
          sumWeight += weight;
        }
      }
    }
    medianCumulWeight = sumWeight / 2;
    for (cumulWeight = 0, stop = 0, x = startX; !stop && x < endX; x++) {  
      for (y = startY; !stop && y < endY; y++) {
        if ((weight = 255 - img(x,y)) >= 0) {  // invert the gray image first and don't include the dark pixels!
          cumulWeight += weight;
	  if (cumulWeight > medianCumulWeight) {
            stop = 1;
            centerX = x;
	  }
        }
      }
    }
  }
  */
  if (1) {  // mean-based centering
    double sumWeightX, sumWeightY, sumWeight;
    int x,y,numPix,weight;
	int tempX,tempY;

    sumWeightX = sumWeightY = sumWeight = 0.0;
    numPix = (endX - startX) * (endY - startY);

    for (y = startY; y < endY; y++) {
      for (x = startX; x < endX; x++) {
	    // problem:  if no pixel passes the condition (ex. white area) then centerX,centerY will be 0,0
        if ((weight = 255 - img(x,y)) > 50) {  // invert the gray image first and don't include the dark pixels!
          sumWeightX += x * weight;
          sumWeightY += y * weight;
          sumWeight += weight;
        }
      }
    }
    tempX = (int)(sumWeightX / (sumWeight + 0.001));
    tempY = (int)(sumWeightY / (sumWeight + 0.001));

	// the computed center is acceptable if it is within the set allowable limits
    if (limitX1 < tempX && tempX < limitX2 &&
        limitY1 < tempY && tempY < limitY2 ) {
	  centerX = tempX;
      centerY = tempY;	
    }
  }
}


void findMarkers( const Image<unsigned char> & img, int dpi, double radiusInches,
                  int & aX, int & aY, int & bX, int & bY, int & cX, int & cY) {

  int rad = 20;  // 25;  // (int)(radiusInches * dpi);
  int ns = 20;   // narrow search radius
  int ns2 = 20;
  int three255 = 255*3;

  int aX0 = aX;
  int aY0 = aY; 
  int bX0 = bX;
  int bY0 = bY; 
  int cX0 = cX;
  int cY0 = cY; 
  
  int x,y, xc,yc,sum,xMin,yMin,sumMin;

  xMin = aX;
  yMin = aY;
  sumMin = 1000000;
  for (x = aX-rad; x < aX+rad; x++) {
    for (y = aY-rad; y < aY+rad; y++) {
      sum = 0;
      for (xc = x-rad; xc < x+rad; xc++)  sum += three255 - img(xc,y) + img(xc,y+1) + img(xc,y-1);
      for (yc = y-rad; yc < y+rad; yc++)  sum += three255 - img(x,yc) + img(x+1,yc) + img(x-1,yc);
      if (sum < sumMin) {
        xMin = x;
        yMin = y;
        sumMin = sum;
      }
    }
  }
  aX = xMin;
  aY = yMin;

  xMin = bX;
  yMin = bY;
  sumMin = 1000000;
  for (x = bX-rad; x < bX+rad; x++) {
    for (y = bY-rad; y < bY+rad; y++) {
      sum = 0;
      for (xc = x-rad; xc < x+rad; xc++)  sum += three255 - img(xc,y) + img(xc,y+1) + img(xc,y-1);
      for (yc = y-rad; yc < y+rad; yc++)  sum += three255 - img(x,yc) + img(x+1,yc) + img(x-1,yc);
      if (sum < sumMin) {
        xMin = x;
        yMin = y;
        sumMin = sum;
      }
    }
  }
  bX = xMin;
  bY = yMin;

  xMin = cX;
  yMin = cY;
  sumMin = 1000000;
  for (x = cX-rad; x < cX+rad; x++) {
    for (y = cY-rad; y < cY+rad; y++) {
      sum = 0;
      for (xc = x-rad; xc < x+rad; xc++)  sum += three255 - img(xc,y) + img(xc,y+1) + img(xc,y-1);
      for (yc = y-rad; yc < y+rad; yc++)  sum += three255 - img(x,yc) + img(x+1,yc) + img(x-1,yc);
      if (sum < sumMin) {
        xMin = x;
        yMin = y;
        sumMin = sum;
      }
    }
  }
  cX = xMin;
  cY = yMin;

  printf("   ... findMarkers\n");
/*
  centerOfMass( img, aX-rad, aY-rad, aX+rad, aY+rad, aX0-rad, aY0-rad, aX0+rad, aY0+rad, aX, aY);
  centerOfMass( img, aX-ns,  aY-ns,  aX+ns,  aY+ns,  aX0-rad, aY0-rad, aX0+rad, aY0+rad, aX, aY);
  centerOfMass( img, aX-ns2, aY-ns2, aX+ns2, aY+ns2, aX0-rad, aY0-rad, aX0+rad, aY0+rad, aX, aY);
  centerOfMass( img, aX-ns2, aY-ns2, aX+ns2, aY+ns2, aX0-rad, aY0-rad, aX0+rad, aY0+rad, aX, aY);
 
  centerOfMass( img, bX-rad, bY-rad, bX+rad, bY+rad, bX0-rad, bY0-rad, bX0+rad, bY0+rad, bX, bY);
  centerOfMass( img, bX-ns,  bY-ns,  bX+ns,  bY+ns,  bX0-rad, bY0-rad, bX0+rad, bY0+rad, bX, bY);
  centerOfMass( img, bX-ns2, bY-ns2, bX+ns2, bY+ns2, bX0-rad, bY0-rad, bX0+rad, bY0+rad, bX, bY);
  centerOfMass( img, bX-ns2, bY-ns2, bX+ns2, bY+ns2, bX0-rad, bY0-rad, bX0+rad, bY0+rad, bX, bY);
  
  centerOfMass( img, cX-rad, cY-rad, cX+rad, cY+rad, cX0-rad, cY0-rad, cX0+rad, cY0+rad, cX, cY);
  centerOfMass( img, cX-ns,  cY-ns,  cX+ns,  cY+ns,  cX0-rad, cY0-rad, cX0+rad, cY0+rad, cX, cY);
  centerOfMass( img, cX-ns2, cY-ns2, cX+ns2, cY+ns2, cX0-rad, cY0-rad, cX0+rad, cY0+rad, cX, cY);
  centerOfMass( img, cX-ns2, cY-ns2, cX+ns2, cY+ns2, cX0-rad, cY0-rad, cX0+rad, cY0+rad, cX, cY);
*/
  printf("   ... findMarkers...done\n");
}


// using k-means clustering, find the value with the highest sum of weights
// value and weight must have the same dimensions
void findDominantValueKMeans(float & mean1, const Image<float> & value, const Image<float> & weight, float valueMin, float valueMax, int numK, int iterations) {
  Image<float> mean;
  Image<float> sum;  //  (sum of weighted values, sum of weights)
  Image<int> label;
  int width,height,npix,c;
  int cMin;  // cluster whose distance from current value is minimum
  float dist,minDist,v;
  int cMax;  // cluster whose sum of weights is maximum
  float maxWeight;
  int p,i;
  
  width = value.width();
  height = value.height();
  npix = width * height;
  
  sum.resize(numK,2);
  mean.resize(numK,1);
  label.resize(width,height);

  // initialize means
  for (c = 0; c < numK; c++) {
    // the (c + 0.5) will put the initial mean in the center of each range
    mean(c,0) = valueMin + (c + 0.5)*(valueMax-valueMin)/numK;
  }
  // converge the means
  for (i = 0; i < iterations; i++) {
    sum.setAll(0.0);
    // for each 'pixel' 
    for (p = 0; p < npix; p++) {   
      v = value[p];
      cMin = 0;
      minDist = fabs(v - mean(cMin,0));  // distance from mean 0
      // find the nearest mean
      for (c = 1; c < numK; c++) {
        dist = fabs(v - mean(cMin,0));
        if (dist < minDist) {
          cMin = c;
          minDist = dist;
        }
      }
      // add to the sum corresponding to the nearest mean
      sum(cMin,0) += v * weight[p];  // sum of weighted values
      sum(cMin,1) += weight[p];      // sum of weights
      label[p] = cMin;  // label the pixel with its closest mean
    }
    // recompute all the means
    for (c = 0; c < numK; c++) {
      mean(c,0) = sum(c,0) / max((float) 1.0,sum(c,1));  // weighted mean
    }  
  }
  // find the mean with the maximum sum of weights
  cMax = 0;
  maxWeight = 0;  // set a low initial value 
  for (c = 1; c < numK; c++) {
    if (sum(c,1) > maxWeight) {
      cMax = c;
      maxWeight = sum(c,1);
    }
  }
  mean1 = mean(cMax,0);  
}


void stretchDocumentHistogram(Image<unsigned char> & grayOutputImage, const Image<unsigned char> & grayInputImage, float blackPercentile, float whitePercentile) {
  Image<int> histogram;
  Image<unsigned char> map;
  int width,height,npix,p,sum,bin,blackGray,whiteGray;
  
  width = grayInputImage.width();
  height = grayInputImage.height();
  npix = width * height;
  // compute histogram
  histogram.resize(256,1);
  histogram.setAll(0);
  for (p = 0; p < npix; p++)
    histogram[grayInputImage[p]]++;
  // compute the remapping of gray values
  map.resize(256,1);
  for (sum = 0, bin = 0; sum < (int)(blackPercentile*npix) && bin < 256; bin++)
    sum += histogram[bin];
  blackGray = bin;
  for (sum = 0, bin = 0; sum < (int)(whitePercentile*npix) && bin < 256; bin++)
    sum += histogram[bin];
  whiteGray = bin;
  for (bin = 0; bin < 256; bin++) {
    if (bin < blackGray)
      map[bin] = 0;
    else if (bin > whiteGray)
      map[bin] = 255;
    else {
      // blackGray is mapped to zero, whiteGray is mapped to 255, everything in between is spread between 0..255
      map[bin] = 255*(bin-blackGray)/(whiteGray-blackGray);
    }
  }
  // remap the intensities of input image to the contrast-stretched mapping
  grayOutputImage.resize(width,height);
  for (p = 0; p < npix; p++)
    grayOutputImage[p] = map[grayInputImage[p]];
}


void fastIntegerMeanFilter(Image<unsigned char> & grayOutputImage, const Image<unsigned char> & grayInputImage, int filterWidth) {
  int width,height,x,y,filterArea,halfWidth,sum;
  int rightLimit,bottomLimit;
  Image<int> horiz;
  
  halfWidth = filterWidth / 2;
  filterArea = filterWidth * filterWidth;
  width = grayInputImage.width();
  height = grayInputImage.height();
  rightLimit = width - filterWidth;
  bottomLimit = height - filterWidth;
  horiz.resize(width,height);
  // compute running sum of each horizontal strip whose length is filterWidth
  for (y = 0; y < bottomLimit; y++) {
    sum = 0;
    for (x = 0; x < filterWidth; x++)
      sum += grayInputImage(x,y);
    horiz(0,y) = sum;
    for (x = 1; x < rightLimit; x++)
      horiz(x,y) = horiz(x-1,y) - grayInputImage(x-1,y) + grayInputImage(x+filterWidth-1,y);
  }  
  // compute running sum of area
  grayOutputImage.resize(width,height);
  grayOutputImage.setAll(255);  // the uncomputed pixels in the boundary are set to white  (255)
  for (x = 0; x < rightLimit; x++) {
    sum = 0;
    for (y = 0; y < filterWidth; y++)
      sum += horiz(x,y);
    grayOutputImage(x+halfWidth,halfWidth) = sum / filterArea;
    for (y = 1; y < bottomLimit; y++) {
      sum = sum - horiz(x,y-1) + horiz(x,y+filterWidth-1);
      grayOutputImage(x+halfWidth,y+halfWidth) = sum / filterArea;
    }
  }  
}


/*
  seek the horizontal line somewhere at the top of the image
  startY   // seeker line starts from this vertical location
  numIterations    // the seeker line converges after this number of iterations
  lineJump   // the seeker line seeks downward
  // the clearance between the top line and the text "Directions..."  is 25 pixels
  numWeights    // numWeights is the height of the vertical window for each seeker point
*/
void seekHorizontalLine(int & x1, int & y1, int & x2, int & y2, const Image<unsigned char> & g, int numPoints, int startY, int numIterations, int lineJump, int numWeights, int imageNumber) {
  Image<int> point;
  Image<unsigned char> grayOutput;
  Image<int> wt;  // guide weights
  int halfNW = numWeights / 2;
  int width = g.width();
  int height = g.height();
  int yWeight;
  int w,sumW, sumYW;
  int sumX,sumXX,sumY,sumYY,sumXY;
  float beta,alpha;
  int xval,yval;
  int iter,p,x,y,xd;
  
  // initialize the guide weights
  wt.resize(numWeights,1);
  wt.setAll(1);

  point.resize(numPoints,2);  // (x,y)  points along the seeker line
  // initialize location of seeker line  
  for (p = 0; p < numPoints; p++) {
    // slice the width into equal parts and put the seeker point in the middle of each slice
    point(p,0) = (int)((p + 0.5) * width/numPoints);
    point(p,1) = startY;  
  }
  iter = 0;
  while (iter < numIterations) {   // while line has not converged
    grayOutput = g;
    // lower the seeker line
    for (p = 0; p < numPoints; p++) {
      point(p,1) += lineJump;
    }
    // release the seeker points
    for (p = 0; p < numPoints; p++) {
      x = point(p,0);
      y = max(4,point(p,1));
      sumW = 0;
      sumYW = 0;
      for (w = 0; w < numWeights; w++) {
        if (y-halfNW+w >= 0 && y-halfNW+w < height)
          yWeight = (255-g(x,y-halfNW+w))*wt(w,0);
        else
          yWeight = 0;
        sumW += yWeight;
        sumYW += yWeight * (y-halfNW+w);
      }
      point(p,1) = sumYW / max(1,sumW);
      if (DEBUG1) {
        // draw the marker for each seeker point
        for (xd = 0; xd < 10; xd++) {
          grayOutput(x+xd,  max(0,min(height-1,point(p,1)-xd))) = 128;
          grayOutput(x+xd+1,max(0,min(height-1,point(p,1)-xd))) = 128;
          grayOutput(x+xd,  max(0,min(height-1,point(p,1)+xd))) = 128;
          grayOutput(x+xd+1,max(0,min(height-1,point(p,1)+xd))) = 128;
        }
      }
    }
    // compute the regression line
    sumX = sumXX = sumY = sumYY = sumXY = 0;
    for (p = 0; p < numPoints; p++) {
      xval = point(p,0);
      yval = point(p,1);
      sumX += xval;
      sumXX += xval*xval;
      sumY += yval;
      sumYY += yval*yval;
      sumXY += xval*yval;
    }
    beta = (1.0*sumXY - 1.0*sumX*sumY/numPoints) / (1.0*sumXX - 1.0*sumX*sumX/numPoints);
    alpha = 1.0*sumY/numPoints - 1.0*beta*sumX/numPoints;
    for (p = 0; p < numPoints; p++) {
      xval = point(p,0);
      point(p,1) = beta * xval + alpha;
      if (DEBUG1) {
        for (xd = 0; xd > -10; xd--) {
          grayOutput(xval+xd,  max(0,min(height-1,point(p,1)-xd))) = 0; // black pointer
          grayOutput(xval+xd+1,max(0,min(height-1,point(p,1)-xd))) = 0;
          grayOutput(xval+xd,  max(0,min(height-1,point(p,1)+xd))) = 0;
          grayOutput(xval+xd+1,max(0,min(height-1,point(p,1)+xd))) = 0;
        }
      }
    }
    if (DEBUG1) writeGrayJpeg( IMAGE_DEBUG_DIRECTORY, imageNumber, "-seek-iter-", iter, grayOutput);
    iter++;
  }
  // "return" the end points of seeker line
  x1 = point(0,0);
  y1 = point(0,1);
  x2 = point(numPoints-1,0);
  y2 = point(numPoints-1,1);
}


void sobelGradientAndAngle(Image<float> & edgeStrengthImage, Image<float> & edgeAngleImage, const Image<unsigned char> & g ) {
  int width,height,x,y,xg,yg;
   
  width = g.width();
  height = g.height();
  edgeStrengthImage.resize(width,height);
  edgeAngleImage.resize(width,height);
  for (y = 0; y < height-2; y++) {
    for (x = 0; x < width-2; x++) {
      xg = -g(x,y) - 2 * g(x,y+1) - g(x,y+2) + g(x+2,y) + 2 * g(x+2,y+1) + g(x+2,y+2);
      yg = -g(x,y) - 2 * g(x+1,y) - g(x+2,y) + g(x,y+2) + 2 * g(x+1,y+2) + g(x+2,y+2);
      edgeStrengthImage(x+1,y+1) = sqrt(1.0*xg*xg + 1.0*yg*yg);
      edgeAngleImage(x+1,y+1) = atan(yg/(xg+0.000001));  // prevent zero denominator
    }
  }
}


void rotateGrayImage(Image<unsigned char> & grayOutputImage, const Image<unsigned char> & grayInputImage, float angleRadians, int xCenter, int yCenter) {
  int width,height,x,y,xs,ys,xp,yp,x0,y0;
  float cosA,sinA;
  
  cosA = cos(angleRadians);
  sinA = sin(angleRadians);
  
  width = grayInputImage.width();
  height = grayInputImage.height();
  grayOutputImage.resize(width,height);
  
  for (xp = 0; xp < width; xp++) {
    for (yp = 0; yp < height; yp++) {
      x0 = xp - xCenter;     // translate to (0,0)
      y0 = yp - yCenter;
      xs = (int)(x0 * cosA + y0 * sinA);   // rotate around the origin
      ys = (int)(- x0 * sinA + y0 * cosA);
      xs = (int)(xs + xCenter);  // translate back to (xCenter,yCenter)
      ys = (int)(ys + yCenter);

      xs = max(0,min(width-1,xs));  // force the source location to within image bounds
      ys = max(0,min(height-1,ys));

      grayOutputImage(xp,yp) = grayInputImage(xs,ys);
    }
  }
}

// translate the image such that the sourceImage(xSource,ySource)  is aligned to (xDest,yDest) in grayOutputImage
//                   x = xSource - xFrameLeft + xOrigin;
// --->  xSource = x + xFrameLeft - xOrigin;
void translateImage(Image<unsigned char> & grayOutputImage, int xDest, int yDest, const Image<unsigned char> & sourceImage, int xSource, int ySource) {
  int width = sourceImage.width();
  int height = sourceImage.height();
  int x,y,xs,ys;
  grayOutputImage.resize(width,height);
  for (y = 0; y < height; y++) {
    for (x = 0; x < width; x++) {
      xs = x + xSource - xDest;
      ys = y + ySource - yDest;
      if (0 <= xs && xs < width && 0 <= ys && ys < height)
        grayOutputImage(x,y) = sourceImage(xs,ys);
      else
        grayOutputImage(x,y) = 255;  // make it paper-white
    }
  }
}



/*
Correction of contrast, rotation and translation:
1. fix document contrast
2. overlay the borders with white  -- to remove any black lines produced during scanning
3. smooth the whole image  (3x3 mean filter)
4. crop out the top portion
     compute SobelX and SobelY gradient magnitude and edge angle
     perform k-means clustering on edge angles with high gradient magnitude to find the angle of rotation Ar
5. Rotate the smoothed document image using angle Ar
6. Find the top edge of the frame using K-means clustering on reverse pixel intensities
7. Find the left edge of the frame using K-means clustering on reverse pixel intensities
8. Translate the document image such that the frame's top line is at y=100 and the frame's left line is at x=100
*/


void correctContrastRotationTranslation_seeker_version(Image<unsigned char> & grayOutputImage, Image<unsigned char> & notSmoothImage, const Image<unsigned char> & grayInputImage, int imageNumber) {
  int width,height,x,y,xp,yp;

  // NOTE:  big  whitened border seems to confuse the seeker
  int borderThickness = 5;   // overlay border with a thick white
  float blackHistogramProportion = 0.005;
  float whiteHistogramProportion = 0.9;
  
  int numPoints = 15;
  int startY = 20;
  int numIterations = 50;
  int lineJump = 5;
  int numWeights = 23;

  int xOrigin = 100;
  int yOrigin = 100;
  
  int sumv, sumxv, sumyv, i, j;
  int seekX1, seekY1, seekX2, seekY2;
  int xFrameLeft,yFrameTop, xFrameRight, yFrameBottom;
  int xSource,ySource;
  float rotationAngle;
  Image<unsigned char> grayImage1,grayImage2;
  Image<unsigned char> rotatedGrayImage;
  Image<unsigned char> transposeGrayImage;
  Image<unsigned char> angleCorrectImage, visualAngleCorrectImage;
  Image<unsigned char> notSmooth1, notSmooth2, notSmooth3;
  
  Image<int> vertSum, horizSum;
  int maxSum;
  int xUpward, xDownward;
  float scaleFactorX, scaleFactorY;
  
  width = grayInputImage.width();
  height = grayInputImage.height();
  
  if (DEBUG1) writeGrayJpeg( IMAGE_DEBUG_DIRECTORY, imageNumber, "-a-raw", 0, grayInputImage);
  // fix document contrast
  stretchDocumentHistogram(grayImage1, grayInputImage, blackHistogramProportion, whiteHistogramProportion);
  if (DEBUG1) writeGrayJpeg( IMAGE_DEBUG_DIRECTORY, imageNumber, "-b-stretch-hist", 0, grayImage1);
  // overlay the borders with white  -- to remove any black lines produced during scanning
  for (x = 0; x < borderThickness; x++) {
    for (y = 0; y < height; y++) {
      grayImage1(x,y) = 255;
      grayImage1(width-1-x,y) = 255;
    }
  }
  for (y = 0; y < borderThickness; y++) {
    for (x = 0; x < width; x++) {
      grayImage1(x,y) = 255;
      grayImage1(x,height-1-y) = 255;
    }
  }
  
  notSmooth1 = grayImage1;
  // smooth the whole image
  if (DEBUG1)  printf("gray image is about to be mean filtered.\n");
  fastIntegerMeanFilter( grayImage2, grayImage1, 5 );
  if (DEBUG1) {
    printf("gray image is mean filtered.\n");
    writeGrayJpeg( IMAGE_DEBUG_DIRECTORY, imageNumber, "-c-mean-filter-", 0, grayImage2);
  }
  
  seekHorizontalLine(seekX1, seekY1, seekX2, seekY2, grayImage2, numPoints, startY, numIterations, lineJump, numWeights, imageNumber);
  yFrameTop = (seekY2+seekY1)/2;
  if (DEBUG1)  printf(" yFrameTop = %d\n", yFrameTop);
  
  rotationAngle = atan(1.0*(seekY2-seekY1)/(seekX2-seekX1));
  if (DEBUG1) printf("The rotation angle is %6.4f degrees\n", rotationAngle*180/3.1415926);
  // rotate the document image around the center of top line
  rotateGrayImage(rotatedGrayImage, grayImage2, -rotationAngle, (seekX2+seekX1)/2,(seekY2+seekY1)/2);
  rotateGrayImage(notSmooth2, notSmooth1, -rotationAngle, (seekX2+seekX1)/2,(seekY2+seekY1)/2);
  if (DEBUG1) writeGrayJpeg( IMAGE_DEBUG_DIRECTORY, imageNumber, "-d-rotated-", 0, rotatedGrayImage);
  
/*
   try finding the left and right borders using vertical projection

  // get the top part of the document image
  transposeGrayImage.resize( height,width );
  for (y = 0; y < height; y++) {
    for (x = 0; x < width; x++) {
      transposeGrayImage(y,x) = rotatedGrayImage(x,y);
    }
  }
  
  seekHorizontalLine(seekX1, seekY1, seekX2, seekY2, transposeGrayImage, numPoints, startY, numIterations, lineJump, numWeights, imageNumber);
  
  xFrameLeft = (seekY2+seekY1)/2;  // we are using Y because the image is transposed
  if (DEBUG1)  printf(" xFrameLeft = %d\n", xFrameLeft);
  // translate the image such that the top-left corner of box is at (X_BOX_CORNER , Y_BOX_CORNER)
  translateImage( angleCorrectImage, X_BOX_CORNER , Y_BOX_CORNER, rotatedGrayImage, xFrameLeft, yFrameTop);
*/
  printf("computing vertSum and horizSum\n");
  angleCorrectImage = rotatedGrayImage;
  // compute vertical reverse-gray projection
  visualAngleCorrectImage = angleCorrectImage;
  vertSum.resize(width,1);
  horizSum.resize(height,1);
  vertSum.setAll(0);
  horizSum.setAll(0);
  for (x = 0; x < width; x++) {
    for (y = 0; y < height; y++) {
      vertSum(x,0) += (255-angleCorrectImage(x,y));
      horizSum(y,0) += (255-angleCorrectImage(x,y));
    }
    // printf("vertSum[%d] = %d ",x,vertSum(x,0));
//    if (vertSum(x,0) > maxSum)
//      maxSum = vertSum(x,0);
  }

  printf("clearing the margins in vertSum and horizSum\n");
  for (x = 0; x < 20; x++)
    vertSum(x,0) = vertSum(width-1-x,0) = 0;   // clear the left and right margins
  for (y = 0; y < 20; y++)
    horizSum(y,0) = horizSum(height-1-y,0) = 0;   // clear the top and bottom margins


  // visualize the vertical gray projection
/*
  for (x = 0; x < width; x++) {
    for (y = height/2; y < height/2+200*vertSum(x,0)/maxSum; y++) {
      visualAngleCorrectImage(x,y) = 128;
    }
  }
*/

  printf("looking for the left line...\n");
  // look for the left line
  xFrameLeft = SEEK_X_WINDOW / 2;
  for (i = 0; i < 100; i++) {
    xFrameLeft += 5;
    sumv = sumxv = 0;
    for (j = 0; j < SEEK_X_WINDOW; j++) {
      x = xFrameLeft - SEEK_X_WINDOW / 2 + j;
      sumv += vertSum(x,0);
      sumxv += x * vertSum(x,0);
    }
    xFrameLeft = sumxv / sumv;
    // printf("xFrameLeft = %d, ",xFrameLeft);
  }
  printf("xFrameLeft = %d \n",xFrameLeft);

  // look for the right line
  xFrameRight = width - SEEK_X_WINDOW / 2 - 2;
  for (i = 0; i < 100; i++) {
    xFrameRight -= 5;
    sumv = sumxv = 0;
    for (j = 0; j < SEEK_X_WINDOW; j++) {
      x = xFrameRight - SEEK_X_WINDOW / 2 + j;
      sumv += vertSum(x,0);
      sumxv += x * vertSum(x,0);
    }
    xFrameRight = sumxv / sumv;
    // printf("xFrameRight = %d, ",xFrameRight);
  }
  printf("xFrameRight = %d \n",xFrameRight);


  // look for the bottom line
  yFrameBottom = height - SEEK_X_WINDOW / 2 - 20;
  for (i = 0; i < 100; i++) {
    yFrameBottom -= 5;
    sumv = sumyv = 0;
    for (j = 0; j < SEEK_X_WINDOW; j++) {
      y = yFrameBottom - SEEK_X_WINDOW / 2 + j;
      sumv += horizSum(y,0);
      sumyv += y * horizSum(y,0);
    }
    yFrameBottom = sumyv / sumv;
  }
  printf(" yFrameTop = %d\n", yFrameTop);
  printf("yFrameBottom = %d \n",yFrameBottom);


/*
  // find the x-location of the box's left line
  for (x = 0; !(vertSum(x,0) < maxSum/3 && vertSum(x+1,0) >= maxSum/3); x++)
    { ; }
  xUpward = x;
  for ( ;  !(vertSum(x,0) > maxSum/3 && vertSum(x+1,0) <= maxSum/3); x++)
    { ; }
  xDownward = x;
  xFrameLeft = (xUpward + xDownward) / 2;

  // find the x-location of the box's right line
  for (x = width-1; !(vertSum(x-1,0) > maxSum/3 && vertSum(x,0) <= maxSum/3); x--)
    { ; }
  xUpward = x;
  for ( ;  !(vertSum(x-1,0) < maxSum/3 && vertSum(x,0) >= maxSum/3); x--)
    { ; }
  xDownward = x;
  xFrameRight = (xUpward + xDownward) / 2;
*/

  translateImage( angleCorrectImage, X_BOX_CORNER , Y_BOX_CORNER, rotatedGrayImage, xFrameLeft, yFrameTop);
  translateImage( notSmooth3,        X_BOX_CORNER , Y_BOX_CORNER, notSmooth2,       xFrameLeft, yFrameTop);
  
  if (DEBUG1) writeGrayJpeg( IMAGE_DEBUG_DIRECTORY, imageNumber, "-e-angle-correct-", 0, visualAngleCorrectImage);

  //  rescale image, radiating from (X_BOX_CORNER),Y_BOX_CORNER)
  scaleFactorX = 1.0 * (X_REF_RIGHT - X_REF_CORNER) / (xFrameRight - xFrameLeft);
  scaleFactorY = 1.0 * (Y_REF_BOTTOM - Y_REF_CORNER) / (yFrameBottom - yFrameTop);
  if (DEBUG1) printf("scaleFactor X and Y = %5.3f %5.3f\n",scaleFactorX, scaleFactorY);
  grayOutputImage.resize(width,height);
  notSmoothImage.resize(width,height);
  
  for (xp = 0; xp < width; xp++) {
    for (yp = 0; yp < height; yp++) {
      // xp = (x - X_BOX_CORNER) * scaleFactor + X_BOX_CORNER;   --> mapping from source to destination
      // x = (xp - X_BOX_CORNER) / scaleFactor + X_BOX_CORNER;
      x = (xp - X_BOX_CORNER) / scaleFactorX + X_BOX_CORNER;
      y = (yp - Y_BOX_CORNER) / scaleFactorY + Y_BOX_CORNER;
      if (0 <= x && x < width && 0 <= y && y < height) {
        grayOutputImage(xp,yp) = angleCorrectImage(x,y);
        notSmoothImage(xp,yp) = notSmooth3(x,y);
      }
      else {
        grayOutputImage(xp,yp) = 255;
        notSmoothImage(xp,yp) = 255;
      }
    }
  }
  if (DEBUG1) writeGrayJpeg( IMAGE_DEBUG_DIRECTORY, imageNumber, "-f-scaled-", 0, grayOutputImage);
}

/*
void correctContrastRotationTranslation(Image<unsigned char> & grayOutputImage, const Image<unsigned char> & grayInputImage, float documentTopPortion, float documentLeftPortion, int imageNumber) {
  int borderThickness = 5;   // overlay border with a thick white
  int width,height,x,y;
  int seekX1, seekY1, seekX2, seekY2;
  int xFrameLeft,yFrameTop;
  int xSource,ySource;
  int xOrigin,yOrigin;
  float rotationAngle;
  Image<unsigned char> grayImage1,grayImage2;
  Image<unsigned char> rotatedGrayImage;
  Image<float> xImage,yImage,edgeStrengthImage,edgeAngleImage;
  Image<float> topCropImage,leftCropImage;

  width = grayInputImage.width();
  height = grayInputImage.height();
  xOrigin = 100;
  yOrigin = 100;
  
  if (DEBUG1) writeGrayJpeg( IMAGE_DEBUG_DIRECTORY, imageNumber, "-a-raw", 0, grayInputImage);
  // fix document contrast
  stretchDocumentHistogram(grayImage1, grayInputImage, 0.005, 0.9);
  if (DEBUG1) writeGrayJpeg( IMAGE_DEBUG_DIRECTORY, imageNumber, "-b-stretch-hist", 0, grayImage1);
  // overlay the borders with white  -- to remove any black lines produced during scanning
  for (x = 0; x < borderThickness; x++) {
    for (y = 0; y < height; y++) {
      grayImage1(x,y) = 255;
      grayImage1(width-1-x,y) = 255;
    }
  }
  for (y = 0; y < borderThickness; y++) {
    for (x = 0; x < width; x++) {
      grayImage1(x,y) = 255;
      grayImage1(x,height-1-y) = 255;
    }
  }
  // smooth the whole image
  if (DEBUG1)  printf("gray image is about to be mean filtered.\n");
  fastIntegerMeanFilter( grayImage2, grayImage1, 5 );
  if (DEBUG1) {
    printf("gray image is mean filtered.\n");
    writeGrayJpeg( IMAGE_DEBUG_DIRECTORY, imageNumber, "-c-mean-filter", 0, grayImage2);
  }
  
  seekHorizontalLine(seekX1, seekY1, seekX2, seekY2, grayImage2, 15, 20,15,5,23, imageNumber);
  
  // get the top part of the document image
  topCropImage.resize( width, height * documentTopPortion);
  for (y = 0; y < topCropImage.height(); y++) {
    for (x = 0; x < width; x++) {
      topCropImage(x,y) = grayImage2(x,y);
    }
  }
  // cropImage( grayImage3, grayImage2, 0,0,width,height * documentTopPortion);
  // compute SobelX and SobelY gradient magnitude and edge angle
  if (DEBUG1) printf("Computing sobel edges and angles...\n");
  sobelGradientAndAngle( edgeStrengthImage, edgeAngleImage, grayImage2 );
  //  find the dominant edge angle of the frame box's top and correct the rotation
  if (DEBUG1) printf("Finding the dominant angle...\n");
  findDominantValueKMeans(rotationAngle, edgeAngleImage, edgeStrengthImage, 0.0, 2*3.14, 10, 3);
  if (DEBUG1) printf("The rotation angle is %6.4f degrees\n", rotationAngle*180/3.1415926);

  // rotate the around the image center  
  rotateGrayImage(rotatedGrayImage, grayImage2, -rotationAngle, width/2,height/2);
  if (DEBUG1) writeGrayJpeg( IMAGE_DEBUG_DIRECTORY, imageNumber, "-d-rotated", 0, rotatedGrayImage);

  // crop the left portion of document and construct weight image where the x values are the weights
  leftCropImage.resize( width * documentLeftPortion,height);
  xImage.resize( leftCropImage.width(),leftCropImage.height());
  for (x = 0; x < leftCropImage.width(); x++) {
    for (y = 0; y < height; y++) {
      leftCropImage(x,y) = 255 - rotatedGrayImage(x,y);
      xImage(x,y) = x;
    }
  }
  // find the frame's left edge
  // NOTE:  a faster option could be to take the vertical sum of gray values
  findDominantValueKMeans( xFrameLeft, xImage, leftCropImage, 0.0, leftCropImage.width(), 10, 3);
    
  // crop the top portion of document and construct weight image where the y values are the weights
  topCropImage.resize( width, height * documentTopPortion);
  yImage.resize( topCropImage.width(),topCropImage.height());
  for (y = 0; y < topCropImage.height(); y++) {
    for (x = 0; x < width; x++) {
      topCropImage(x,y) = 255-rotatedGrayImage(x,y);  // reverse intensities
      yImage(x,y) = y;
    }
  }
  // find the frame's top edge
  // NOTE:  a faster option could be to take the horizontal sum of gray values
  findDominantValueKMeans( yFrameTop, yImage, topCropImage, 0.0, topCropImage.width(), 10, 3);
  // correct the translation to (X_BOX_CORNER,Y_BOX_CORNER)
  translateImage( grayOutputImage, X_BOX_CORNER,Y_BOX_CORNER, rotatedGrayImage, xFrameLeft,yFrameTop);
  if (DEBUG1) writeGrayJpeg( IMAGE_DEBUG_DIRECTORY, imageNumber, "-e-corrected", 0, grayOutputImage);
  
}
*/

/*
// compute the location of point in input image
void computeLocation( int ref1X, int ref1Y, int ref2X, int ref2Y, int targetX, int targetY, 
                 int aX, int aY, int bX, int bY, int & whereX, int & whereY)  {

  double angleRef, angleForm;  // orientation angle of reference and input form
  double angleTarget;  // target's angle from the horizontal of rectified form
  double angle;
  double distTarget;
  double xNew, yNew, x2, y2;

  angleRef = atan((ref2Y-ref1Y)/(ref2X-ref1X+0.001));
  angleForm = atan((bY-aY)/(bX-aX+0.001));
  angleTarget = atan((targetY-ref1Y)/(targetX-ref1X)) - angleRef;

  distTarget = sqrt(pow(targetY-ref1Y,2)+pow(targetX-ref1X,2));  // euclidean distance
  xNew = aX + distTarget;
  yNew = aY;  

  // rotate xNew,yNew around the origin (aX,aY) of input form
  // translate the origin to (0,0)
  x2 = xNew - aX;
  y2 = yNew - aY;

  // rotate with respect to the origin
  angle = angleTarget + angleForm;
  xNew = x2 * cos(angle) - y2 * sin(angle);
  yNew = x2 * sin(angle) + y2 * cos(angle);
      
  // translate the origin of coordinate back to (xCenter,yCenter)      
  xNew += aX;
  yNew += aY;

  whereX = (int) xNew;
  whereY = (int) yNew;
}
*/

void alignInputForm( Image<unsigned char> & alignedGray, Image<unsigned char> & notSmoothOutput, RGBImage & outFrame, const Image<unsigned char> & inputGray, Image<unsigned char> & notSmoothInput, int refWidth, int refHeight, int ref1X, int ref1Y, int ref2X, int ref2Y, int ref3X, int ref3Y, int aX, int aY, int bX, int bY, int cX, int cY) {

// void alignInputForm( Image<unsigned char> & alignedGray, RGBImage & outFrame, const Image<unsigned char> & inputGray, int refWidth, int refHeight, int ref1X, int ref1Y, int ref2X, int ref2Y, int ref3X, int ref3Y, int aX, int aY, int bX, int bY, int cX, int cY) {

  double angleRef, angleForm;  // orientation angle of reference and input form
  double a, moveProp;
  int x,y,x0,y0,xa,ya, p,npix;
  int dx,dy,xs,ys;
  double xp,yp,cosa,sina;
  double scaleFactor;
  RGBImage outFrame2;
  Image<unsigned char>  alignedGray2, notSmoothOutput2;
  
  // scaling is part of alignment
  scaleFactor = sqrt((ref1X-ref2X)*(ref1X-ref2X)+(ref1Y-ref2Y)*(ref1Y-ref2Y))/ sqrt((aX-bX)*(aX-bX) + (aY-bY)*(aY-bY));
  
  angleRef = atan((ref2Y-ref1Y)/(ref2X-ref1X+0.001));
  angleForm = atan((bY-aY)/(bX-aX+0.001));
  a = -angleForm + angleRef;
  cosa = cos(a);
  sina = sin(a);
  printf("Rotate angle = %0.3f degrees, Translation dx,dy = %d,%d,  Scale factor = %0.4f\n",a*180.0/3.1415926, aX-ref1X,aY-ref1Y, scaleFactor);
  
  // copy inputGray to outFrame and put an arrow on the reference markers
  outFrame.resize(inputGray.width(),inputGray.height());
  npix = inputGray.width() * inputGray.height();
  for (p = 0; p < npix; p++)
    outFrame[p] = COLOR_RGB(inputGray[p],inputGray[p],inputGray[p]);
  drawBox( outFrame, COLOR_RGB(255,128,0), 3, aX, aY, aX+20, aY+20);
  drawBox( outFrame, COLOR_RGB(255,128,0), 3, bX, bY, bX+20, bY+20);
  
  outFrame2 = outFrame;
/*    
  alignedGray.resize(refWidth,refHeight);
  alignedGray.setAll(255);
  for (x = 0; x < inputGray.width(); x++) {
    for (y = 0; y < inputGray.height(); y++) {
      x0 = x - aX;
      y0 = y - aY;
      xp = x0 * cosa - y0 * sina;
      yp = x0 * sina + y0 * cosa;
      xa = (int)(xp + ref1X + 0.5);
      ya = (int)(yp + ref1Y + 0.5);
      if (xa >=0 && xa < refWidth && 
          ya >= 0 && ya < refHeight) {
        alignedGray(xa,ya) = inputGray(x,y);
      }
    }
  }
*/  
  alignedGray.resize(refWidth,refHeight);
  alignedGray.setAll(255);
  notSmoothOutput.resize(refWidth,refHeight);
  notSmoothOutput.setAll(255);
  outFrame.resize(refWidth,refHeight);
  outFrame.setAll(COLOR_RGB(255,255,255));
  for (x = 0; x < inputGray.width(); x++) {
    for (y = 0; y < inputGray.height(); y++) {
      x0 = x - aX;     // upper-left cross is set as the origin
      y0 = y - aY;
      xp = scaleFactor * (x0 * cosa - y0 * sina);  // multiply this by scaling factor 
      yp = scaleFactor * (x0 * sina + y0 * cosa);
      xa = (int)(xp + ref1X + 0.5);
      ya = (int)(yp + ref1Y + 0.5);
      if (xa >=0 && xa < refWidth && 
          ya >= 0 && ya < refHeight) {
        alignedGray(xa,ya) = inputGray(x,y);
        notSmoothOutput(xa,ya) = notSmoothInput(x,y);
        outFrame(xa,ya) = outFrame2(x,y);
      }
    }
  }
  if (DEBUG2) printf("   ... alignInputForm...align with two markers...done\n");
  
  // compute the new location of bottom marker
  x0 = cX - aX;     // upper-left cross is set as the origin
  y0 = cY - aY;
  xp = scaleFactor * (x0 * cosa - y0 * sina);  // multiply this by scaling factor 
  yp = scaleFactor * (x0 * sina + y0 * cosa);
  cX = (int)(xp + ref1X + 0.5);
  cY = (int)(yp + ref1Y + 0.5);
  drawBox( outFrame, COLOR_RGB(255,128,0), 3, cX, cY, cX+20, cY+20);    // caused range errors!
  if (DEBUG2) printf("   ... new location of third marker is %d,%d \n", cX,cY);
  
  alignedGray2 = alignedGray;
  notSmoothOutput2 = notSmoothOutput;
  outFrame2 = outFrame;
  
  alignedGray.setAll(255);
  notSmoothOutput.setAll(255);
  // warp the image
  dx = ref3X - cX;
  dy = ref3Y - cY;
  for (y = 0; y < refHeight; y++) {
    moveProp = max(0.0, min(1.0 , 1.0 * (y - ref1Y) / (ref3Y - ref1Y)));
    ys = (int)(y - dy * moveProp);
    for (x = 0; x < refWidth; x++) {
      // (xs,ys) is where to fetch the data  "source"
      xs = (int)(x - dx * moveProp);
      if (xs >= 0 && xs < refWidth && ys >=0 && ys < refHeight) {
        alignedGray(x,y) = alignedGray2(xs,ys);
        notSmoothOutput(x,y) = notSmoothOutput2(xs,ys);
        outFrame(x,y) = outFrame2(xs,ys);
      }
    }
  }
 
  if (DEBUG2) printf("   ... alignInputForm...done\n");
  
}
