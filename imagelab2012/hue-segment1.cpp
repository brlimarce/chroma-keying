
/* 
  Seed shape analysis
*/


#define IMAGE_RANGE_CHECK

#include "image.h"
#include "jpegio.h"
#include "color.h"
#include "binary.h"
#include "tools.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// findObjects

// Find the largest fitting circle C in the binary image B, and let R be the radius of C.
// take  f * R  be radius of the structuring element for loose opening (or stat-opening) the binary
// image B.  Loose opening can be done using loose erosion followed by ordinary dilation.


namespace eigen1 {
  float val, x, y;
}

namespace eigen2 {
  float val, x, y;
}


class Seed {
public:
  Seed()  {   }
  ~Seed()  {   }
  
  int mXLeft, mYTop;  // the top-left corner of the seed's bounding box relative to original image
  int mArea;          // number of pixels of the seed's silhouette
  float mRotationAngle;   // the seed's angle of deviation from the flat position
  int mXCenter;    // the seed's X center-of-mass (inside the seed bounding box)
  int mYCenter;    // the seed's Y center-of-mass (inside the seed bounding box)

  int mLengthMajorAxis;   // length and endpoints based on the major axis (1st eigenvector)
  int mX1MajorAxis, mY1MajorAxis;
  int mX2MajorAxis, mY2MajorAxis;

  int mX1MinorWidest, mY1MinorWidest;
  int mX2MinorWidest, mY2MinorWidest;
  inline int widthWidest()  { return abs(mY2MinorWidest -mY1MinorWidest); }

  int mLengthFarthestPair;  // length and endpoints based on farthest pair
  int mX1FarthestPair, mY1FarthestPair;
  int mX2FarthestPair, mY2FarthestPair;


  Image<unsigned char> seedBinary;
  RGBImage seedRGB1, seedRGB2, seedRGB2Rotated;
  RGBImage tempRGB3;
  Image<unsigned char> seedBinary1, seedBinary2, seedBinary2Rotated;

  // process the seed whose binary image is seedBinary1, and whose top-left corner is in (x0,y0)
  // of the accession color image mInputRGB
  void processSeed( Image<unsigned char> seedBinary1, RGBImage & mInputRGB, int x0, int y0 );

  inline int getWidth() { return seedBinary2Rotated.width(); }
  inline int getHeight() { return seedBinary2Rotated.height(); }

  // startProp,endProp are [0..1], marking the start/end of the seed silhouette for computing length;  ex. startProp = 0.01,  endProp = 0.99
  void computeLength(float startProp, float endProp);

// compute the seed's "width", defined as the widest part of the silhouette
// perpendicular to the major axis
  void computeWidthWidest();

  // from the "flat" seed image, rotate and project a point x,y from the seed's rotated binary
  // coordinate system to the accession's RGBImage visualization; scale accordingly
  void xyAcc(int & xa, int & ya, int x, int y, int scale);

  // overlay the annotations on top of visualization image, which is a larger by "scale" factor
  // over the resolution of the annotations
  void annotateSeedOverlayAccession( RGBImage & accessionRGB, int scale );
};


void Seed::processSeed( Image<unsigned char> seedBinary1, RGBImage & mInputRGB, int x0, int y0 )  {
  using namespace eigen1;
  using namespace eigen2;
//  Image<unsigned char>  seedBinary2;
  int wSeed1 = seedBinary1.width();
  int hSeed1 = seedBinary1.height();
  int wSeedR = sqrt(wSeed1*wSeed1 + hSeed1*hSeed1);
  int hSeedR = wSeedR;
  int i;
  RGBImage binaryRGB1, binaryRGB2 , tempRGB1, tempRGB2;
  mXLeft = x0;   // the top-left corner of the seed's bounding box relative to mInputRGB
  mYTop = y0;

  binaryCenterOfMass( mXCenter, mYCenter, seedBinary1 );  // center of mass

  seedBinary2.resize(wSeedR,hSeedR);
  seedBinary2.setAll( 0 );
  overlayImageT( seedBinary2, seedBinary1, wSeedR/2 - mXCenter, hSeedR/2 - mYCenter);
  eigenvectors2D( eigen1::val, eigen1::x, eigen1::y, eigen2::val, eigen2::x, eigen2::y, seedBinary2);
  printf("Eigenvalues = %3.2f,%3.2f, First eigenvector = %3.2f,%3.2f\n", eigen1::val,eigen2::val, eigen1::x, eigen1::y);
     
  cropImageRGB( seedRGB1, mInputRGB, x0,y0,x0+wSeed1,y0+hSeed1);
  seedRGB2.resize( wSeedR,hSeedR );
  seedRGB2.setAll( COLOR_RGB(255,255,255) );
  overlayImageRGB( seedRGB2, seedRGB1, wSeedR/2 - mXCenter, hSeedR/2 - mYCenter);

  mRotationAngle = atan(eigen1::y / eigen1::x);  // angle of the first eigenvector of the covariance matrix
  rotateImageT(seedBinary2Rotated, seedBinary2, -mRotationAngle, wSeedR/2, hSeedR/2, (unsigned char) 0);
  mArea = 0;
  for (i = 0; i < getWidth()*getHeight(); i++)    // area of the seed in pixels
    mArea += seedBinary2Rotated[i];   
  rotateRGB(seedRGB2Rotated, seedRGB2, -mRotationAngle, wSeedR/2, hSeedR/2);
  printf("Seed rotated...\n");

  binaryRGB1.resize( seedBinary1.width(), seedBinary1.height() );
  binaryRGB2.resize( seedBinary2Rotated.width(), seedBinary2Rotated.height() );
  makeRGBFromBinary( binaryRGB1, seedBinary1, 0, 1 );   // 1 == RGB is same scale as binary
  makeRGBFromBinary( binaryRGB2, seedBinary2Rotated, 0, 1);
  concatImageRGB( tempRGB1, binaryRGB1, binaryRGB2);
  concatImageRGB( tempRGB2, seedRGB1, seedRGB2Rotated);
  concatImageRGB( tempRGB3, tempRGB1, tempRGB2);
}


// startProp,endProp are [0..1], marking the start/end of the seed silhouette for computing length;  ex. startProp = 0.01,  endProp = 0.99
void Seed::computeLength(float startProp, float endProp) {
  int x,y,x1,x2,sum;

  mX1MajorAxis = mX2MajorAxis = -1;   // -1 == not yet computed
  sum = 0;
  for (x = 0; x < getWidth(); x++) {
    for (y = 0; y < getHeight(); y++) {
      sum += seedBinary2Rotated(x,y);
      if (mX1MajorAxis == -1 && sum > mArea * startProp)
        mX1MajorAxis = x;
      if (mX1MajorAxis != -1 && mX2MajorAxis == -1 && sum > mArea * endProp)
        mX2MajorAxis = x;      
    }
  }
  printf("area = %d, x1 = %d, width/2 = %d, x2 = %d\n", mArea, mX1MajorAxis, getWidth()/2, mX2MajorAxis);

  int xAcc = mXLeft + mXCenter;
  int yAcc = mYTop + mYCenter;

  x1 = mX1MajorAxis - getWidth()/2;  // centered on the origin (0,0)
  mX1MajorAxis = cos(mRotationAngle) * x1 + xAcc;
  mY1MajorAxis = sin(mRotationAngle) * x1 + yAcc;
  x2 = mX2MajorAxis - getWidth()/2;   // centered on the origin (0,0)
  mX2MajorAxis = cos(mRotationAngle) * x2 + xAcc;
  mY2MajorAxis = sin(mRotationAngle) * x2 + yAcc;

  mLengthMajorAxis = (int) sqrt((mX2MajorAxis - mX1MajorAxis)*(mX2MajorAxis - mX1MajorAxis)
                       +(mY2MajorAxis - mY1MajorAxis)*(mY2MajorAxis - mY1MajorAxis));

  // compute length and endpoints based on farthest pair
  farthestPairOfPoints( mX1FarthestPair, mY1FarthestPair, mX2FarthestPair, mY2FarthestPair, mLengthFarthestPair, seedBinary2Rotated);
}


// compute the seed's "width", defined as the widest part of the silhouette
// perpendicular to the major axis
void Seed::computeWidthWidest() {
  int x, y1, y2;

  int maxWidth = 0;
  for (x = 0; x < getWidth(); x++) {
    y1 = 0;
    y2 = getHeight() - 1;
    while (y1 < y2 && seedBinary2Rotated(x,y1) != 1)
      y1++;
    while (y1 < y2 && seedBinary2Rotated(x,y2) != 1)
      y2--;
    if (y2 - y1 + 1 > maxWidth)  {
      maxWidth = y2 - y1 + 1;
      mX1MinorWidest = mX2MinorWidest = x;
      mY1MinorWidest = y1;
      mY2MinorWidest = y2;
    }
  }

}


// from the "flat" seed image, rotate and project a point x,y from the seed's rotated binary
// coordinate system to the accession's RGBImage visualization; scale accordingly
void Seed::xyAcc(int & xa, int & ya, int x, int y, int scale) {
  int xAcc = mXLeft + mXCenter;
  int yAcc = mYTop + mYCenter;
  int x0 = x - getWidth() / 2;
  int y0 = y - getHeight() / 2;
  float sinA = sin(mRotationAngle);
  float cosA = cos(mRotationAngle);
  xa = (int) (scale * (x0 * cosA - y0 * sinA + xAcc));
  ya = (int) (scale * (x0 * sinA + y0 * cosA + yAcc));
}


// overlay the annotations on top of visualization image, which is a larger by "scale" factor
// over the resolution of the annotations
void Seed::annotateSeedOverlayAccession( RGBImage & accessionRGB, int scale )  {
  int n = 100, i, x, y;
//  Image<float> point;
  int x1,y1,x2,y2;
  int black = COLOR_RGB(0,0,0), violet = COLOR_RGB(255,128,255);
  int orange = COLOR_RGB(255,128,0), green = COLOR_RGB(0,255,0), white = COLOR_RGB(255,255,255);
  int xAcc = mXLeft + mXCenter;
  int yAcc = mYTop + mYCenter;
  int crossWidth = 30;

  // draw the major axis as a black line
  x1 = cos(mRotationAngle) * 1.25 * getWidth()/2.0 + xAcc;
  y1 = sin(mRotationAngle) * 1.25 * getWidth()/2.0 + yAcc;
  x2 = xAcc - (x1 - xAcc);
  y2 = yAcc - (y1 - yAcc);
  drawLineRGB( accessionRGB, scale*x1,scale*y1,scale*x2,scale*y2,black,6 );


  // draw the seed's end points along the major axis
  drawLineRGB( accessionRGB, scale*mX1MajorAxis, scale*mY1MajorAxis-crossWidth, scale*mX1MajorAxis, scale*mY1MajorAxis+crossWidth, green,6 );
  drawLineRGB( accessionRGB, scale*mX1MajorAxis-crossWidth, scale*mY1MajorAxis, scale*mX1MajorAxis+crossWidth, scale*mY1MajorAxis, green,6 );
  drawLineRGB( accessionRGB, scale*mX2MajorAxis, scale*mY2MajorAxis-crossWidth, scale*mX2MajorAxis, scale*mY2MajorAxis+crossWidth, green,6 );
  drawLineRGB( accessionRGB, scale*mX2MajorAxis-crossWidth, scale*mY2MajorAxis, scale*mX2MajorAxis+crossWidth, scale*mY2MajorAxis, green,6 );

  // draw the widest part of the seed silhouette
  xyAcc( x1,y1,mX1MinorWidest,mY1MinorWidest, scale);
  xyAcc( x2,y2,mX2MinorWidest,mY2MinorWidest, scale);
  drawLineRGB( accessionRGB, x1,y1,x2,y2, white,10 );

  // draw the two farthest points of the silhouette as orange crosses
  xyAcc( x1,y1,mX1FarthestPair,mY1FarthestPair, scale);
  xyAcc( x2,y2,mX2FarthestPair,mY2FarthestPair, scale);
  drawLineRGB( accessionRGB, x1, y1-crossWidth, x1, y1+crossWidth, orange,6 );
  drawLineRGB( accessionRGB, x1-crossWidth, y1, x1+crossWidth, y1, orange,6 );
  drawLineRGB( accessionRGB, x2, y2-crossWidth, x2, y2+crossWidth, orange,6 );
  drawLineRGB( accessionRGB, x2-crossWidth, y2, x2+crossWidth, y2, orange,6 );

}


class Accession {
public:
  Accession()  {
    mSeed = new Seed[1];
  }
  ~Accession()  {
    delete [] mSeed;
  }

  char mFileName[200];
  RGBImage mInputRGB;    // the input scanned image at 2400 dpi
  RGBImage mScaledInputRGB;  // the input image scaled to 600 dpi
  RGBImage mOutputRGB1;  // annotates the input color image 
  RGBImage mOutputRGB2;  // annotates the binary image

  int  mImageWidth;
  int  mImageHeight;

  int   mNumSeeds;
  Seed*   mSeed;

  void processAccessionImage(char *fileName);

};



void Accession::processAccessionImage(char *fileName) {

  Image<unsigned char> binary1, binary2, binary3, binary4, binary5, struc1, struc2;
  ConnectedComponents accessionComponents;
  int x, s, x0, y0, w, h;
  char outputFileName[200];

  strcpy(mFileName,fileName);
  readJpeg( mInputRGB, mFileName );

  scaleImageRGB( mScaledInputRGB, mInputRGB, mInputRGB.width() / 4, mInputRGB.height() / 4);
  mImageHeight = mScaledInputRGB.height();
  mImageWidth  = mScaledInputRGB.width();
  mOutputRGB1 = mScaledInputRGB;
  colorSegment( binary1, mScaledInputRGB);  // binarization based on dominant hue (background); in color.h

  // Clean the binary image
  binaryMedianFilter( binary2, binary1, 7, 7 );
  binaryMakeShape( struc1, 1, 21 );

  // Remove the awn
  binaryLooseErosion( binary3, binary2, struc1, struc1.width()/2, struc1.width()/2, 0.90);
  binaryDilation(binary4, binary3, struc1, struc1.width()/2, struc1.width()/2);

  // Get the intersection of the two binary images.
  binary5.resize(mImageWidth,mImageHeight);
  for (x = 0; x < mImageWidth*mImageHeight; x++)  {
    binary5[x] = binary2[x] * binary4[x];
  }

  binaryMakeShape( struc2, 1, 5 );  
  binaryErosion( binary2, binary5, struc2, 2, 2);
  binaryDilation( binary3, binary2, struc2, 2, 2);

  makeRGBFromBinary( mInputRGB, binary3, 5, 4 );  // 5 == boundary method of visualization;  4 ==

  accessionComponents.analyzeBinary( binary3, EIGHT_CONNECTED );
  mNumSeeds = accessionComponents.getNumComponents();
  printf("Number of connected components = %d\n", mNumSeeds);

  mOutputRGB2.resize( mImageWidth,mImageHeight );
  makeRGBFromBinary( mOutputRGB2, binary3, 0, 1 );

  delete [] mSeed;
  mSeed = new Seed[mNumSeeds];

  for (s = 0; s < mNumSeeds; s++) {
    accessionComponents.getBoundary( s,x0,y0,w,h );
    printf("Component %d, width = %d, height = %d\n", s, w, h);
    mSeed[s].processSeed( accessionComponents.getComponentBinary(s), mInputRGB, x0,y0  );

    mSeed[s].computeLength( 0.0001, 0.9999);
    mSeed[s].computeWidthWidest();
//    mSeed[s].annotateSeedOverlayAccession( mOutputRGB1, 4 );  // 2nd parameter is the scale of the 
    mSeed[s].annotateSeedOverlayAccession( mInputRGB, 4 );  // 2nd parameter is the scale of the 

//    sprintf( outputFileName, "%s_%03d.output.jpg", mFileName, s);
//    writeJpeg( mSeed[s].tempRGB3, outputFileName, 90 ); 
  }

  sprintf( outputFileName, "%s.redstripe.jpg", mFileName);   // first visualization
//  writeJpeg( mOutputRGB1, outputFileName, 90 ); 
  writeJpeg( mInputRGB, outputFileName, 60 ); 

  sprintf( outputFileName, "%s.rotated.jpg", mFileName);     // second visualization
  writeJpeg( mOutputRGB2, outputFileName, 90 ); 

}



int main (int argc, char *argv[]) {

   Accession accession1;

   char outputFileName[200];
   int markColor = COLOR_RGB(255,0,0);

   accession1.processAccessionImage( argv[1] );

   // save the seed info array in the CSV file

   // save the visualization images

}




