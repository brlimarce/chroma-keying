
/* 
   Edge detection using 
      1. The image gradient
      2. The image gradient (first derivative) and zero-crossing (second-derivative)
  The program reads in a color JPEG image, 
  and highlights the significant edges.
*/


#define IMAGE_RANGE_CHECK

#include "image.h"
#include "jpegio.h"
#include "filter.h"
#include "math.h"
#include "tools.h" //<algo.h>


int main () {
  RGBImage img, edgeVisual, gradientVisual, zeroCrossVisual;
  Image<double> meanMask, laplacian, sobelX, sobelY;
  Image<double> grayScale, secondDeriv;
  Image<double> filteredIntensity, gradientX, gradientY;
  int x,y,height,width, gradientGray;
  double gradient, gx, gy;

  // create the Sobel gradient masks
  sobelX.resize(3,3);   // sobel X mask
  sobelX(0,0) = -1; sobelX(1,0) = 0; sobelX(2,0) = 1; 
  sobelX(0,1) = -2; sobelX(1,1) = 0; sobelX(2,1) = 2; 
  sobelX(0,2) = -1; sobelX(1,2) = 0; sobelX(2,2) = 1; 

  sobelY.resize(3,3);   // sobel Y mask
  sobelY(0,0) = -1; sobelY(1,0) = -2; sobelY(2,0) = -1; 
  sobelY(0,1) =  0; sobelY(1,1) =  0; sobelY(2,1) =  0; 
  sobelY(0,2) =  1; sobelY(1,2) =  2; sobelY(2,2) =  1; 

  // create the Laplacian operator
  laplacian.resize(3,3);   // sobel X mask
  laplacian(0,0) =  0; laplacian(1,0) = -1; laplacian(2,0) =  0; 
  laplacian(0,1) = -1; laplacian(1,1) =  4; laplacian(2,1) = -1; 
  laplacian(0,2) =  0; laplacian(1,2) = -1; laplacian(2,2) =  0; 

  // first-derivative threshold
  // the gradient magnitude threshold separates
  // the significant edges from the non-sgnificant edges in the image
  double gradientMagnitudeThreshold = 50;

  // read the JPEG image
  readJpeg( img, "images/game.jpg" );
  width = img.width();
  height = img.height();
  edgeVisual.resize(width,height);  // RGB visualization of the significant edges
  gradientVisual.resize(width,height);  // RGB visualization of the gradient
  zeroCrossVisual.resize(width,height);  // RGB visualization of the zero crossings

  // convert the input image to gray scale
  grayScale.resize( width,height );
  for (x = 0; x < width; x++ )
    for (y = 0; y < height; y++ )
      grayScale(x,y) = (RED(img(x,y)) + GREEN(img(x,y)) + BLUE(img(x,y))) / 3;

  // smooth the image first to reduce noise
  meanMask.resize(3,3);
  meanMask.setAll( 1.0/9.0 );
  convolveDouble( filteredIntensity, grayScale, meanMask );

  // compute the gradientX by convolving filteredIntensity with the mask sobelX
  convolveDouble( gradientX, filteredIntensity, sobelX );  
  // compute the gradientY by convolving filteredIntensity with the mask sobelY
  convolveDouble( gradientY, filteredIntensity, sobelY );
  
  // compute the second derivative image by convolving filteredIntensity with Laplacian operator
  convolveDouble( secondDeriv, filteredIntensity, laplacian );  
  
  // compute the gradient magnitude and mark the significant edges
  for (x = 0; x < width-1; x++) {
    for (y = 0; y < height-1; y++) {
      gx = gradientX(x,y);
      gy = gradientY(x,y);
      gradient = pow( pow(gx,2) + pow(gy,2) , 0.5 );  // magnitude of the gradient (gx,gy)
      gradientGray = min(255,(int) gradient); 
      gradientVisual(x,y) = COLOR_RGB(gradientGray,gradientGray,gradientGray);

      edgeVisual(x,y) = COLOR_RGB(RED(img(x,y))/3, GREEN(img(x,y))/3, BLUE(img(x,y))/3);
      zeroCrossVisual(x,y) = edgeVisual(x,y);
      // check if its a significant edge:   a high first-derivative gradient magnitude
      if (gradient > gradientMagnitudeThreshold)  {  
        edgeVisual(x,y) = COLOR_RGB(255,255,255);   // a significant edge pixel
        // find if this pixel is a zero-crossing
        if  ((int)(secondDeriv(x,y)) * (int)(secondDeriv(x+1,y)) < 0 
          || (int)(secondDeriv(x,y)) * (int)(secondDeriv(x,y+1)) < 0) {
          zeroCrossVisual(x,y) = COLOR_RGB(255,128,0);  // color it orange
        }
      }       
    }
  }

  // write the visualization of the edges
  writeJpeg( edgeVisual, "images/output/edges.jpg", 100 );
  writeJpeg( zeroCrossVisual, "images/output/zerocross.jpg", 100 );
  writeJpeg( gradientVisual, "images/output/gradient.jpg", 100 );
}
