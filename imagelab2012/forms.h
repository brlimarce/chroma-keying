

#ifndef FORMS_H
#define FORMS_H

#include "image.h"
#include "stdio.h"

// target location of the input form's top-left corner
#define X_BOX_CORNER 50
#define Y_BOX_CORNER 50

// current location of the reference document's top-left corner and x-location of right line
#define X_REF_CORNER 78  // was 52
#define Y_REF_CORNER 88  // was 47
#define X_REF_RIGHT 1135    // was 1206
#define Y_REF_BOTTOM 1532    // 
//     these should be variables in the csv file!!!

void drawBox( RGBImage & img, int color, int thick, int x1, int y1, int x2, int y2);

void translateImage(Image<unsigned char> & grayOutputImage, int xDest, int yDest, const Image<unsigned char> & sourceImage, int xSource, int ySource);

void rotateGrayImage(Image<unsigned char> & grayOutputImage, const Image<unsigned char> & grayInputImage, float angleRadians, int xCenter, int yCenter);

void centerOfMass( const Image<unsigned char> & img, int startX, int startY, int endX, int endY, int limitX1, int limitY1, int limitX2, int limitY2, int & centerX, int & centerY);

void seekHorizontalLine(int & x1, int & y1, int & x2, int & y2, const Image<unsigned char> & g, int numPoints, int startY, int numIterations, int lineJump, int numWeights, int imageNumber);

void findMarkers( const Image<unsigned char> & img, int dpi, double margin,
                int & aX, int & aY, int & bX, int & bY, int & cX, int & cY);

void findDominantValueKMeans(float & mean1, const Image<float> & value, const Image<float> & weight, float valueMin, float valueMax, int numK, int iterations);

void stretchDocumentHistogram(Image<unsigned char> & grayOutputImage, const Image<unsigned char> & grayInputImage, float blackPercentile, float whitePercentile);

void correctContrastRotationTranslation(Image<unsigned char> & grayOutputImage, const Image<unsigned char> & grayInputImage, float documentTopPortion, float documentLeftPortion, int imageNumber);

// an initial document "correction" is done:
//    1. measure the angle of the box's top line and correct the rotation
//    2. translate the document image such that the box's top-left corner is at (X_BOX_CORNER , Y_BOX_CORNER)
void correctContrastRotationTranslation_seeker_version(Image<unsigned char> & grayOutputImage, Image<unsigned char> & notSmoothImage,  const Image<unsigned char> & grayInputImage, int imageNumber);

void writeGrayJpeg(char* fileNamePrefix, int imageNumber, char* fileNameTag, int iterNumber, const Image<unsigned char> & grayInputImage);

/*
// compute the location of point in input image
void computeLocation( int ref1X, int ref1Y, int ref2X, int ref2Y, int targetX, int targetY, int aX, int aY, int bX, int bY, int & whereX, int & whereY);
*/

void alignInputForm( Image<unsigned char> & alignedGray, Image<unsigned char> & notSmoothOutput, RGBImage & outFrame, const Image<unsigned char> & inputGray, Image<unsigned char> & notSmoothInput, int refWidth, int refHeight, int ref1X, int ref1Y, int ref2X, int ref2Y, int ref3X, int ref3Y, int aX, int aY, int bX, int bY, int cX, int cY);




#endif   
