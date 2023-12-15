
#ifndef BINARY_H
#define BINARY_H

#include "image.h"

// area of the binary image
int binaryArea( const Image<unsigned char> & img );

void binaryGetFirstPixel( int & xFirst, int & yFirst, const Image<unsigned char> & img);

int binaryFollowBoundary(RGBImage & boundaryRGB, float & perimeter, const Image<unsigned char> & img, int startX, int startY);

Image<unsigned char> binaryComplement( const Image<unsigned char> & img );

void binaryMedianFilter( Image<unsigned char> & outputImage, const Image<unsigned char> & inputImage, int filterWidth, int filterHeight);

void binarizeGrayImage( Image<unsigned char> & outputImage, const Image<unsigned char> & 
inputImage, int method, float parameter1, float parameter2);

void binaryCenterOfMass( int & xc, int & yc, const Image<unsigned char> & binaryImage );

// -------------------------------------------------------
//     Binary morphological operations
// -------------------------------------------------------

void binaryErosion(Image<unsigned char> & outputImage, const Image<unsigned char> & img, const Image<unsigned char> & strucElem, int originX, int originY );

void binaryDilation(Image<unsigned char> & outputImage, const Image<unsigned char> & img, const Image<unsigned char> & strucElem, int originX, int originY );

// instead of perfect fitting, binaryLooseErosion applies a statistical proportion "proportionMatch" (ex. 95%) to test whether the structuring element fits in the current location
void binaryLooseErosion(Image<unsigned char> & outputImage, const Image<unsigned char> & inputImage, const Image<unsigned char> & strucElem, int originX, int originY, float proportionMatch );

// -----------------------------------------------------------------------
//  - Make a binary image with a particular shape.  This is useful for making structuring elements.
//  - shapeType = 0 for square, 1 for circle
//  - For making structuring elements, "width" should be an odd number.
// -----------------------------------------------------------------------
void binaryMakeShape(Image<unsigned char> & outputImage, int shapeType, int width);

// given the binary image "inputImage", compute the two eigenvectors
void eigenvectors2D( float & ev1, float & ex1, float & ey1, float & ev2, float & ex2, float & ey2, Image<unsigned char> & inputImage);

// return the two points on the binary image that are farthest from each other
void farthestPairOfPoints(int & x1, int & y1, int & x2, int & y2, int & length, Image<unsigned char> & binary);

// -------------------------------------------------------
//     Connected component tools class
// -------------------------------------------------------

enum CONNECTED_TYPE { FOUR_CONNECTED = 4, EIGHT_CONNECTED = 8 };

class ConnectedComponents { 
private:
   int          mNumComponents;

public:
   Image<int>   mTagImage; // -1 == not part of any CC

   // mBbox and mCount use the Image<int> class as data structure
   Image<int>  mBbox;   // each row == [x1,y1,w,h]
   Image<int>  mCount;  // each row == [CC number, pixel count]

   ConnectedComponents() { mNumComponents = 0;  }
   ~ConnectedComponents() { }

   int getNumComponents() { return mNumComponents; }

   // erase all connected components information -- main use is for freeing memory
   void reset() 
     { mNumComponents = 0;  mTagImage.resize( 1,1 ); 
       mBbox.resize(1,1); mCount.resize(1,1);  }

   // returns the number of connected components (numComponents) in binary;
   // mTagImage( x,y ) becomes the nametag == [0..mNumComponents)
   // uses breadth-first-search/region-growing

   int analyzeBinary( const Image<unsigned char> & binary, int connectType);

   inline void getBoundary(int c, int & x, int & y, int & w, int & h) { 
     x = mBbox(c,0); y = mBbox(c,1); 
     w = mBbox(c,2); h = mBbox(c,3); 
	//printf("alskd");
   }

   // return the binary image of the n-th component
   Image<unsigned char> getComponentBinary( int n );

   inline int componentHeight(int c)
              { return mBbox( c,3 ); }
   inline int componentWidth( int c)
              { return mBbox( c,2 ); }

   // return a multi-random-colored image of all connected components
   RGBImage randomColors();
};


#endif   

