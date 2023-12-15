
#ifndef FILTER_H
#define FILTER_H

#include "image.h"

// binaryMedianFilter() is found in binary.h

template <class T>
void orderStatFilter_QUICKSORT( T *A, int left, int right )  {
   T   v,temp;
   int i,j;
   if (right > left)  {
      v = A[right];  i = left - 1;  j = right;
      for (;;)  {
         while (A[++i] < v)  ;
         while (A[--j] > v)  ;
         if (i >= j)  break;
         temp = A[i];  A[i] = A[j];  A[j] = temp;
      }
      temp = A[i];  A[i] = A[right];  A[right] = temp;
      orderStatFilter_QUICKSORT( A, left, i-1  );
      orderStatFilter_QUICKSORT( A, i+1,  right);
   }
}


template <class T>
Image<T> orderStatFilter( const Image<T> & Img, int maskw, int percent) {

   Image<T> outimg;
   T *list;

   int ht,wd,i,j,rad,ilist,m,n,ne,num;
   ht = Img.height();
   wd = Img.width();
   outimg = Img; 

   rad   = maskw / 2;
   num   = maskw * maskw;
   ilist = (int) (percent / 100.0 * (num-1));
   list  = new T[num];

   // for each pixel
   for (i = rad;  i <= ht-rad-1; i++)
   for (j = rad;  j <= wd-rad-1; j++)  {
      ne = 0;
      for (m = -rad; m <= rad; m++)
      for (n = -rad; n <= rad; n++)
         list[ne++] = Img( j+n,i+m );  // put the neighborhood in a list

      // sort the list
      orderStatFilter_QUICKSORT( list, 0, num-1 );

      // get the percent-th element from the list
      outimg(j,i)  =  list[ilist];
   }
	
   delete [] list;
   return outimg;
}


void fastIntegerMeanFilter( Image<int> & outputImage, const Image<int> & inputImage, int filterWidth);

void convolveDouble( Image<double> & outputImage, const Image<double> & inputImage, const Image<double> & mask);

void correlationDouble( Image<double> & outputImage, const Image<double> & inputImage, const Image<double> & pattern);

void histogramEqualize(Image<unsigned char> & histeq, const Image<unsigned char> & gray, int startX, int startY, int endX, int endY, int percent);

void stretchGrayHistogram(Image<unsigned char> & grayOutputImage, const Image<unsigned char> & grayInputImage, float blackPercentile, float whitePercentile);


#endif   
