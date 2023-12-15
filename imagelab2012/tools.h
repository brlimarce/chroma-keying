

#ifndef TOOLS_H
#define TOOLS_H

#include "image.h"
#include "math.h"

void convertSingleChannelJPEG( Image<unsigned char> & gray, const RGBImage & inputFrame);

// =================== statistical and math tools ==========================

// The following were copied from http://www.cplusplus.com/reference/algorithm/max/
template <class T> const T& max ( const T& a, const T& b ) {
  return (b<a)?a:b;     // or: return comp(b,a)?a:b; for the comp version
}

template <class T> const T& min ( const T& a, const T& b ) {
  return (b>a)?a:b;     // or: return comp(b,a)?a:b; for the comp version
}

template <class T> const T& square ( const T& a ) {
  return ((T)(a)*(T)(a));
}

inline int withinRange(int lower, int upper, int value) {
  if (value >= lower && value <= upper)
    return 1;
  else
    return 0;
}

void grayscaleUChar( Image<unsigned char> & gray, const RGBImage & img );

int euclideanDistance2d(int x1, int y1, int x2, int y2);

inline float radiansFromDegrees(float degrees)
{  return (3.14159 * degrees / 180.0);  }

inline float degreesFromRadians(float radians)
{  return (180.0 * radians / 3.14159);  }

template <class T> 
float varianceT( const Image<T> & vector ) {
  float sumx = 0.0;
  float sumxx = 0.0;
  int i, n = vector.width()*vector.height();
  for (i = 0; i < n; i++) {
    sumx += vector[i];
    sumxx += (1.0*vector[i])*vector[i];
  }
  return ((sumxx - sumx/n*sumx)/n);
}

template <class T> 
int indexMaxT( const Image<T> & vector ) {
  int i, imax = 0, n = vector.width()*vector.height();
  for (i = 0; i < n; i++) {
    if (vector[i] > vector[imax])
      imax = i;
  }
  return imax;
}

// =================== warping tools ==========================

template <class T>
int cropImageT( Image<T> & outputImage, const Image<T> & inputImage, int x1, int y1, int x2, int y2) {
  int x,y;
  if (x1 >= x2 || y1 >= y2 || x1 < 0 || y1 < 0 || x2 >= inputImage.width() || y2 >= inputImage.height()) {
    printf("cropImageRGB: ERROR IN CROPPING BOUNDS!\n");
    return 0;
  }
  outputImage.resize(x2-x1,y2-y1);
  for (x = x1; x < x2; x++) {
    for (y = y1; y < y2; y++) {
      outputImage(x-x1,y-y1) = inputImage(x,y);
    }
  }
  return 1;
}

int cropImageRGB( RGBImage & outputImage, const RGBImage & inputImage, int x1, int y1, int x2, int y2);

int concatImageRGB( RGBImage & outputImage, const RGBImage & inputImage1, const RGBImage & inputImage2);

void translateRGB(RGBImage & shiftedRGB, const RGBImage & img, int translateX, int translateY);

void rotateRGB(RGBImage & outputImage, const RGBImage & inputImage, float angleRadians, int xCenter, int yCenter);

template <class T>
void rotateImageT(Image<T> & outputImage, const Image<T> & inputImage, float angleRadians, int xCenter, int yCenter, T outsideValue) {
  int width,height,x,y,xs,ys,xp,yp,x0,y0;
  float cosA = cos(angleRadians);
  float sinA = sin(angleRadians);
  width = inputImage.width();
  height = inputImage.height();
  outputImage.resize(width,height);
  outputImage.setAll( outsideValue ); // after rotation, the values outside the rotated image
  for (xp = 0; xp < width; xp++) {
    for (yp = 0; yp < height; yp++) {
      x0 = xp - xCenter;     // translate to (0,0)
      y0 = yp - yCenter;
      xs = (int)(x0 * cosA + y0 * sinA + xCenter);   // rotate around the origin and
      ys = (int)(- x0 * sinA + y0 * cosA + yCenter);  // translate back to (xCenter,yCenter)
      if (xs >= 0 && ys >= 0 && xs < width && ys < height)
        outputImage(xp,yp) = inputImage(xs,ys);
    }
  }
}


// overlays the generic image with top-left corner placed at x0,y0 of the outputImage
// outputImage is 
template <class T>
void overlayImageT(Image<T> & outputImage, const Image<T> & inputImage, int x0, int y0) {
  int x,y;
  int width = inputImage.width();
  int height = inputImage.height();
  int width2 = outputImage.width();
  int height2 = outputImage.height();
  for (x = 0; x < width; x++) {
    for (y = 0; y < height; y++) {
      if (x0+x >= 0 && x0+x < width2 && y+y0 >= 0 && y+y0 < height2)
        outputImage(x0+x,y0+y) = inputImage(x,y);
    }
  }
}


void scaleImageRGB( RGBImage & outputImage, const RGBImage & inputImage, int targetWidth, int targetHeight);

void verticalProjection( Image<int> & vertProjection, const Image<unsigned char> & inputImage);

// =================== visualization and drawing tools ==========================

void overlayRuler(RGBImage & outputImageRGB, int yStart, int pixPerMM );

void overlayImageRGB( RGBImage & outputImageRGB, const RGBImage & inputImageRGB, int x0, int y0);

void overlayTextRGB( RGBImage & outputImageRGB, char *text, int x0, int y0);

void drawBoxRGB( RGBImage & inputImageRGB, int x1, int y1, int x2, int y2, int color, int thickness );

void drawLineRGB( RGBImage & inputImageRGB, int x1, int y1, int x2, int y2, int color, int thickness);

// void overlayStringRGB( RGBImage & outputImageRGB, char *inputString, int & xCursor, int & yCursor);

// binary to RGB

// gray scale to RGB

void grayscaleDouble( Image<double> & gray, const RGBImage & img );

void makeGrayFromRGB( Image<unsigned char> & grayOutput, const RGBImage & colorImage );

void makeRGBFromGray( RGBImage & colorOutputRGB, Image<unsigned char> & grayImage);

/* 
  makeRGBFromBinary()
  The dimensions of colorOutputRGB and its initial content should be set by the calling function.
  binary1 and colorOutputRGB are assumed to have the same dimensions.
  method == 0   [0 == white, 1 == black]
  method == 1   [0 == black, 1 == original]
  method == 2   [0 == white, 1 == original]
  method == 3   [0 == gray, 1 == original]
  method == 4   [0 == gray, 1 == original+striped+boundary outline]
  method == 5   [0 == gray, 1 == original+boundary outline]
  method == 6   [0 == darkened original, 1 == original]

  scale == the scale of colorOutputRGB relative to binary1
         1 = same scale; 2 = twice the dimensions, ...
*/
void makeRGBFromBinary( RGBImage & colorOutputRGB, Image<unsigned char> & binary1, int method, int scale);

int fileExist(char *fileName);


#endif     // TOOLS_H











