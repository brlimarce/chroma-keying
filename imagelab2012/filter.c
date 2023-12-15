
#include "image.h"
#include "math.h"
#include "stdio.h"

void fastIntegerMeanFilter( Image<int> & outputImage, const Image<int> & inputImage, int filterWidth, int filterHeight)  {
  int h = inputImage.height(),
      w = inputImage.width();
  int x,y,sum;
  int area = filterWidth * filterHeight;
  Image<int> horizSum;
  outputImage.resize( w,h );
  outputImage.setAll( 0 );
  horizSum.resize(h,1);
  for (x = 0; x < w-filterWidth; x++) {   // compute vertical sums
    if (x == 0) {
      horizSum.setAll(0);
      for (y = 0; y < h; y++) {    // compute first column horizontal sums
        for (x = 0; x < filterWidth; x++)  
          horizSum[y] += inputImage(x,y);
      }
    } else {
    for (y = 0; y < h; y++)
      horizSum[y] = horizSum[y] - inputImage(x-1,y) + inputImage(x+filterWidth-1,y);
    }
    for (sum = 0, y = 0; y < filterHeight; y++)   // the top window 
      sum += horizSum[y];
    outputImage(x + filterWidth/2, filterHeight/2) = sum / area;
    for (y = 1; y < h-filterHeight; y++) {    // window slides down
      sum = sum - horizSum[y-1] + horizSum[y+filterWidth-1];
      outputImage(x + filterWidth/2,y + filterWidth/2) = sum / area;
    }
  }
}

/*
this is the old one

void fastIntegerMeanFilter( Image<int> & outputImage, const Image<int> & inputImage, int filterWidth)  {
  int h = inputImage.height(),
      w = inputImage.width();
  int x,y,sum;
  int area = filterWidth * filterWidth;
  Image<int> horizSum;
  outputImage.resize( w,h );
  outputImage.setAll( 0 );
  horizSum.resize(w,h);
  horizSum.setAll(0);
  for (y = 0; y < h; y++) {    // compute horizontal sums
    for (x = 0; x < filterWidth; x++)
      horizSum(0,y) += inputImage(x,y);
    for (x = 1; x < w-filterWidth; x++)
      horizSum(x,y) = horizSum(x-1,y) - inputImage(x-1,y) + inputImage(x+filterWidth-1,y);
  }
  for (x = 0; x < w-filterWidth; x++) {   // compute vertical sums
    for (sum = 0, y = 0; y < filterWidth; y++)
      sum += horizSum(x,y);
    outputImage(x + filterWidth/2, filterWidth/2) = sum / area;
    for (y = 1; y < h-filterWidth; y++) {
      sum = sum - horizSum(x,y-1) + horizSum(x,y+filterWidth-1);
      outputImage(x + filterWidth/2,y + filterWidth/2) = sum / area;
    }
  }
}
*/


void convolveDouble( Image<double> & outputImage, const Image<double> & inputImage, 
                     const Image<double> & mask)  {
  int x,y,m,n,ht,wd,maskWidth,maskHeight,halfWidth,halfHeight;
  double sum;

  outputImage = inputImage;
  wd = inputImage.width();
  ht = inputImage.height();
  maskWidth = mask.width();  // mask width
  maskHeight = mask.height();  // mask height
  halfWidth = maskWidth / 2;
  halfHeight = maskHeight / 2; 

  for (x = 0;  x <= wd-maskWidth; x++)
  for (y = 0;  y <= ht-maskHeight; y++)  {
    sum = 0.0;
    for (m = 0; m < maskWidth; m++)
    for (n = 0; n < maskHeight; n++) {
      sum += inputImage(x+m,y+n) * mask(m,n);
    }
    outputImage(x+halfWidth,y+halfHeight) = sum;
  }
}


void correlationDouble( Image<double> & outputImage, const Image<double> & inputImage, 
                     const Image<double> & pattern)  {
  int x,y,m,n,ht,wd,patWidth,patHeight,halfWidth,halfHeight;
  double sumA,sumB, sumA2, sumB2, sumAB;
     // A stands for the input image, B is for the pattern image
  double  valueA, valueB, area;

  double corr, maxCorr, minCorr;
  maxCorr = -1000000.0;
  minCorr = 1000000.0;
   
  outputImage.resize( wd = inputImage.width(), ht = inputImage.height() );
  outputImage.setAll( 0.0 );
  patWidth = pattern.width();  // pattern image width
  patHeight = pattern.height();  // pattern image height
  halfWidth = patWidth / 2;
  halfHeight = patHeight / 2; 
  area = patHeight * patWidth;

  sumB = sumB2 = 0.0;
  for (m = 0; m < patWidth; m++)  {
    for (n = 0; n < patHeight; n++) {
      valueB = pattern(m,n);
      sumB += valueB;
      sumB2 += valueB * valueB;
    }  
  }
  
  for (x = 0;  x <= wd-patWidth; x++)  {
    for (y = 0;  y <= ht-patHeight; y++)  {
      sumA = sumA2 = sumAB = 0.0;
      for (m = 0; m < patWidth; m++)  {
        for (n = 0; n < patHeight; n++) {
          valueA = inputImage(x+m,y+n);
          valueB = pattern(m,n);
          sumA += valueA;
          sumA2 += valueA * valueA;
          sumAB += valueA * valueB;
        }
      }
      corr = outputImage(x+halfWidth,y+halfHeight) = (sumAB - sumA*sumB/area)/area 
                         / sqrt((sumA2 - sumA*sumA/area)/area * (sumB2 - sumB*sumB/area)/area);
      if (corr > maxCorr)  maxCorr = corr;
      if (corr < minCorr)  minCorr = corr;

    }
    printf("x = %d of %d, minCorr = %0.3f, maxCorr = %0.3f\n", x, wd-patWidth, minCorr, maxCorr);
  }
}


void histogramEqualize(Image<unsigned char> & histeq, const Image<unsigned char> & gray,
   int startX, int startY, int endX, int endY, int percent)  {
   int height = gray.height();
   int width  = gray.width();
   int h, x,y, numSamples, histSum;
   int hist[256], Ymap[256];
   // compute the histogram from the sub-image
   for (h = 0; h < 256; h++)
     hist[h] = 0;    // initialize the histogram
   for (x = startX; x < endX; x++) {
     for (y = startY; y < endY; y++) {
       hist[gray(x,y)]++;
     }
   }
     
   // remap the Ys, use the maximum contrast (percent == 100) 
   // based on histogram equalization
   numSamples = (endX - startX) * (endY - startY);   // # of samples that contributed to the histogram
   histSum = 0;
   for (h = 0; h < 256; h++) {
     histSum += hist[h];
     Ymap[h] = histSum * 255 / numSamples;
   }
    
   // if desired contrast is not maximum (percent < 100), then adjust the mapping
   if (percent < 100) {
     for (h = 0; h < 256; h++) {
       Ymap[h] = h + ((int)Ymap[h] - h) * percent / 100;
     }
   }   
  
  histeq.resize(width,height);
  // enhance the region by remapping the intensities
  for (y = startY; y < endY; y++) {
    for (x = startX; x < endX;  x++) {
      histeq(x,y) = Ymap[gray(x,y)];  // set the new value of the gray value
    }
  }
}


void stretchGrayHistogram(Image<unsigned char> & grayOutputImage, const Image<unsigned char> & grayInputImage, float blackPercentile, float whitePercentile) {
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
