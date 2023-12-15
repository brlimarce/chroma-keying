/* 
  K-means clustering:  Finding the K major color cluster in a color image
  19 September 2007
  
  usage:      ./d_kmeans <input_image> <number of clusters>
  example:    ./d_kmeans sample.jpg 5
*/

#define IMAGE_RANGE_CHECK

#include "image.h"
#include "jpegio.h"
#include "tools.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int square(int x) {
  return x*x;
}

int main(int argc, char *argv[]) {
   int x,y,c,npix,pix,p,cMin,minDist,dist,iter, height,width;
   char fileName[100];
   
   int K = atoi(argv[2]);
   int numIterations = 15;
   
   RGBImage inputImage, outputImage;
   Image<int> label;   // the cluster number that this pixel belongs to
   Image<int> mean,sum,nc;

   // read the JPEG file
   readJpeg( inputImage, argv[1] );
   height = inputImage.height();
   width  = inputImage.width();
   npix = width*height;
   printf("Image dimensions are %d x %d\n",width,height);

   mean.resize(K,3);   // mean of R, G and B
   
   // set each mean as a random RGB
   for (c = 0; c < K; c++) {
     mean(c,0) = rand() % 255;  // R
     mean(c,1) = rand() % 255;  // G
     mean(c,2) = rand() % 255;  // B
   }

   sum.resize(K,3);
   nc.resize(K,1);
   label.resize(width,height);

   // for a fixed number of iterations   
   for (iter = 0; iter < numIterations; iter++) {
     printf("Iteration = %d of %d\n",iter,numIterations);
     sum.setAll(0);
     nc.setAll(0);
     // for each pixel
     for (p = 0; p < npix; p++) {   
       pix = inputImage[p];
       cMin = 0;
       minDist = square(RED(pix)-mean(cMin,0)) + square(GREEN(pix)-mean(cMin,1)) + square(BLUE(pix)-mean(cMin,2));  // distance from mean 0
       // find the nearest mean
       for (c = 1; c < K; c++) {
         dist = square(RED(pix)-mean(c,0)) + square(GREEN(pix)-mean(c,1)) + square(BLUE(pix)-mean(c,2));
         if (dist < minDist) {
           cMin = c;
           minDist = dist;
         }
       }
       // add to the sum corresponding to the nearest mean
       sum(cMin,0) += RED(pix);
       sum(cMin,1) += GREEN(pix);
       sum(cMin,2) += BLUE(pix);
       nc[cMin]++;
       label[p] = cMin;  // label the pixel with its closest mean
     }
     // recompute all the means
     for (c = 0; c < K; c++) {
       mean(c,0) = sum(c,0) / max(1,nc[c]);
       mean(c,1) = sum(c,1) / max(1,nc[c]);
       mean(c,2) = sum(c,2) / max(1,nc[c]);
     }
   }
   
   // write all K color clusters
   system("mkdir images");
   system("mkdir images/kmeans");
   system("rm images/kmeans/*.*");
   for (c = 0; c < K; c++) {   // for each cluster
     outputImage.resize(width*2,height);
     outputImage.setAll(COLOR_RGB(128,128,128));
     for (x = 1; x < width-1; x++) {
       for (y = 1; y < height-1; y++) {
         outputImage(x,y) = inputImage(x,y);
         outputImage(x+width,y) = COLOR_RGB(128,128,128);
         if (label(x,y) == c) {    // copy the color, plus do cross-shaped binary dilation
           outputImage(x+width,y) = inputImage(x,y);
           outputImage(x+width,y-1) = inputImage(x,y-1);
           outputImage(x+width,y+1) = inputImage(x,y+1);
           outputImage(x+width-1,y) = inputImage(x-1,y);
           outputImage(x+width+1,y) = inputImage(x+1,y);
         }
       }
     }       
     sprintf(fileName,"images/kmeans/%04d.jpg",c);
     writeJpeg(outputImage,fileName,80);
   }
}
