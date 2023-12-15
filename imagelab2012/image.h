
#ifndef IMAGE_H
#define IMAGE_H

// #include <iostream.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

template <class T>     // an image of type T
                       // use only simple types like int, unsigned char
                       // double or float, etc.
class Image {

private:
   T*     pixel;
   int    mHeight;
   int    mWidth;
   char   mName[100];

public:
   Image() { pixel = new T[1];  mHeight = mWidth = 0; }
   Image(const Image<T> & img );
   Image(int w, int h);

   Image<T> & operator=(const Image<T> & img);        // copy constructor

   // OLD element access operator
   inline T & operator () (int x, int y) const {
#ifdef IMAGE_RANGE_CHECK
     //      cout << "Test range check..." << endl;
      if (y >= mHeight || x >= mWidth || y < 0 || x < 0) {
         printf("Cannot access pixel (%d,%d) of \"%s\" Image[%d x %d]!\n",x,y,mName,mWidth,mHeight);
         exit(1);
      }
#endif
      return pixel[y * mWidth + x];
   }

   // one-dimensional element access operator;
   inline T & operator [] (int i) const {
#ifdef IMAGE_RANGE_CHECK
      if (i >= mWidth * mHeight || i < 0) {
         printf("Cannot access pixel [%d] of \"%s\" Image[%d]!\n",i,mName,mWidth*mHeight);
//         cout << "Cannot access pixel [" << i << 
//                 "] of Image[" << mWidth * mHeight << "]!" << endl;
         exit(1);
      }
#endif
      return pixel[i];
   }

   // destructor
   ~Image(){  delete [] pixel;  }

   void resize(int w, int h);
   void setAll( T x );    // set the values
   void setName(char *name) {
     strcpy(mName,name);
   }

   inline int height() const { return mHeight; }
   inline int width()  const { return mWidth;  }
   inline int isInside(int x, int y) {
     if (x > 0 && y > 0 && x < mWidth && y < mHeight) return 1;
     else return 0;
   }
   T max();
   T min();
};


// --------------------------------------------------------------------

template <class T>
Image<T>::Image( const Image<T> & img ) {   // important for functions returning Image
   int h = img.height(), w = img.width(), i;
   int n = w * h;
   pixel = new T[1];
   resize( w,h );
   for (i = 0; i < n; i++)   pixel[i] = img[i];
   // for a faster alternative, replace previous line with the one below ; 
   //    and... you must #include <string.h>
   //   memcpy( pixel, & img[0], sizeof(T) * w * h );  
}


// set the image size only
template <class T>
Image<T>::Image( int w, int h ) {
   pixel = new T[1];
   resize( w,h );  
}


// ---------------- Copy constructor -----------------------------------
template <class T>                            
Image<T> & Image<T>::operator=(const Image<T> & img) {
   int w = img.width(), h = img.height();
   int i, n = w * h;
   if ( this != &img )  {  // don't copy to yourself
      resize( w, h );
      for (i = 0; i < n; i++)   pixel[i] = img[i];
      // for a faster alternative, replace previous line with the one below ; 
      //    and... you must #include <string.h>
      // memcpy( pixel, & Img[0] , sizeof(T) * ht * wd ); 
   }
   return *this;
}


//----------- Allocates memory for an image, no initialization -----------
template <class T>
void Image<T>::resize(int w, int h) {
   int i;
   delete [] pixel;

   if (w * h == 0) {
      printf("CreateImage: invalid dimensions (%d x %d)\n",w,h);
      //cout << "CreateImage:  invalid dimensions (" << w << " x " <<
      //        h << ")..." << endl;
      exit(1);
   }

   pixel = new T[w*h];
   mWidth = w;  mHeight = h;   
}


// set the values of an image
template <class T>
void Image<T>::setAll( T x ) {
   int n = width() * height();
   for (int i = 0; i < n; i++)  pixel[i] = x;
} 

// return the maximum value of the image
template <class T>
T Image<T>::max() { 
  int i,imax,n = height()*width();
  for (i=imax=0; i < n; i++)  
    if (pixel[i] > pixel[imax])  
      imax = i;
  return pixel[imax];
}

// return the minimum value of the image
template <class T>
T Image<T>::min() { 
  int i,imin,n = height()*width();
  for (i=imin=0; i < n; i++)  
    if (pixel[i] < pixel[imin])  
      imin = i;
  return pixel[imin];
}




#define RED(pixel) ((pixel) & 0xff)
#define GREEN(pixel) (((pixel)>>8) & 0xff)
#define BLUE(pixel) (((pixel)>>16) & 0xff)
#define COLOR_RGB(r,g,b) (((b)<<16)+((g)<<8)+(r))



class RGBImage : public Image<int> {

public:
  RGBImage() { };
  ~RGBImage() { };
  inline void setPix( int x, int y, unsigned char r, unsigned char g, unsigned char b) {
     (*this)(x,y) = COLOR_RGB(r,g,b);
  }
  inline void setPix( int i, unsigned char r, unsigned char g, unsigned char b) {
     (*this)[i] = COLOR_RGB(r,g,b);
  }
};




#endif














