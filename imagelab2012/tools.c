
/* 
Tools
*/

#define IMAGE_RANGE_CHECK

#include "image.h"
#include "tools.h"
#include "math.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

//========================================================================
void convertSingleChannelJPEG( Image<unsigned char> & gray, const RGBImage & inputFrame) {
  int width = inputFrame.width();
  int height = inputFrame.height();
  int x,y, pix,x3;
//  gray.resize(height,width);
  gray.resize(width,height);
  for (y = 0; y < height; y++)  {
    for (x = 0; x < width/3; x++) {
      pix = inputFrame(x,y);
      x3 = x * 3;
      // if (x3 % 2 && y % 2)
        gray(x3,y) = RED(pix);
      // if ((x3+1) % 2 && y % 2)
        gray(x3+1,y) = GREEN(pix);
      // if ((x3+2) % 2 && y % 2)
        gray(x3+2,y) = BLUE(pix);
   
/*      
      // if (x3 % 2 && y % 2)
        gray(y,width - 1 - x3) = RED(pix);
      // if ((x3+1) % 2 && y % 2)
        gray(y,width - 1 - (x3+1)) = GREEN(pix);
      // if ((x3+2) % 2 && y % 2)
        gray(y,width - 1 - (x3+2)) = BLUE(pix);
*/
    }
  }
}


void grayscaleUChar( Image<unsigned char> & gray, const RGBImage & img )  {
  int i, wd = img.width(), ht = img.height(), npix = wd*ht;
  gray.resize( wd,ht );
  for (i = 0; i < npix; i++ )
    gray[i] = (((int) 0) + RED(img[i]) + GREEN(img[i]) + BLUE(img[i])) / 3;
}

//========================================================================
int euclideanDistance2d(int x1, int y1, int x2, int y2) {
  int distdist = (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2);
  return (int)(sqrt(distdist) + 0.5);   // round to the nearest integer
}


//========================================================================
int fileExist(char *fileName) {
#ifdef DEBUG1
  printf("fileExist():  checking if jpeg file exists...\n");
#endif
  FILE * pFile;
  pFile = fopen (fileName,"rb");
  if (pFile!=NULL)  {
    fclose (pFile);
    return 1;
  }
  return 0;
}


int cropImageRGB( RGBImage & outputImage, const RGBImage & inputImage, int x1, int y1, int x2, int y2){
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


int concatImageRGB( RGBImage & outputImage, const RGBImage & inputImage1, const RGBImage & inputImage2) {
  int x,y;
  int width1 = inputImage1.width(), height1 = inputImage1.height();
  int width2 = inputImage2.width(), height2 = inputImage2.height();
  outputImage.resize(width1+width2,max(height1,height2));
  outputImage.setAll( COLOR_RGB(255,255,255) );
  for (x = 0; x < width1; x++) {
    for (y = 0; y < height1; y++) {
      outputImage(x,y) = inputImage1(x,y);
    }
  }
  for (x = 0; x < width2; x++) {
    for (y = 0; y < height2; y++) {
      outputImage(x+width1,y) = inputImage2(x,y);
    }
  }
  return 1;
}

//========================================================================
void translateRGB(RGBImage & shiftedRGB, const RGBImage & img, int translateX, int translateY) {
  int width = img.width();
  int height = img.height();
  int x,y, sourceX, sourceY;
  shiftedRGB.resize(width,height);
  shiftedRGB.setAll( COLOR_RGB(0,0,0) );
  for (x = 0; x < width; x++)  {
    for (y = 0; y < height; y++)  {
      sourceX = x - translateX;
      sourceY = y - translateY;
      if (sourceX >= 0 && sourceX < width && sourceY >= 0 && sourceY < height)
        shiftedRGB(x,y) = img(x,y);
    }
  }      
}


//========================================================================
void rotateRGB(RGBImage & outputImage, const RGBImage & inputImage, 
     float angleRadians, int xCenter, int yCenter) {
  int width,height,x,y,xs,ys,xp,yp,x0,y0;
  float cosA,sinA;
  
  cosA = cos(angleRadians);
  sinA = sin(angleRadians);
  
  width = inputImage.width();
  height = inputImage.height();
  outputImage.resize(width,height);
  outputImage.setAll( COLOR_RGB(255,255,255) );
  
  for (xp = 0; xp < width; xp++) {
    for (yp = 0; yp < height; yp++) {
      x0 = xp - xCenter;     // translate to (0,0)
      y0 = yp - yCenter;
      xs = (int)(x0 * cosA + y0 * sinA);   // rotate around the origin
      ys = (int)(- x0 * sinA + y0 * cosA);
      xs = (int)(xs + xCenter);  // translate back to (xCenter,yCenter)
      ys = (int)(ys + yCenter);

//      xs = max(0,min(width-1,xs));  // force the source location to within image bounds
//      ys = max(0,min(height-1,ys));
      if (xs >= 0 && xs < width && ys >= 0 && ys < height)
        outputImage(xp,yp) = inputImage(xs,ys);
    }
  }
}


//========================================================================
void scaleImageRGB( RGBImage & outputImage, const RGBImage & inputImage, 
     int targetWidth, int targetHeight) {
  int xTarget,yTarget,xSource,ySource;
  int width = inputImage.width();
  int height = inputImage.height();
  outputImage.resize(targetWidth,targetHeight);

  for (xTarget = 0; xTarget < targetWidth; xTarget++) {
    for (yTarget = 0; yTarget < targetHeight; yTarget++) {
      xSource = xTarget * width / targetWidth;
      ySource = yTarget * height / targetHeight;
      outputImage(xTarget,yTarget) = inputImage(xSource,ySource);
    }
  }
}  

//========================================================================
void verticalProjection( Image<int> & vertProjection, const Image<unsigned char> & inputImage) {
  int width = inputImage.width();
  int height = inputImage.height();
//  printf("verticalProjection:  width = %d, height = %d\n",width,height);
  int sum,x,y;
  vertProjection.resize(width,1);
//  printf("verticalProjection:  computing projection\n");
  for (x = 0; x < width; x++) {
    sum = 0;
    for (y = 0; y < height; y++) {
      sum += inputImage(x,y);
    }
    vertProjection[x] = sum;
  }
}

//========================================================================
void overlayRuler(RGBImage & outputImageRGB, int yStart, int pixPerMM ) {
  int width = outputImageRGB.width();
  const int xDivs = 10;
  int xStart = 10;
  int color[10];
  int c,x,m,n;
  color[0] = COLOR_RGB(255,255,255); color[1] = COLOR_RGB(192,0,0); color[2] = COLOR_RGB(0,192,0); color[3] = COLOR_RGB(0,0,192); color[4] = COLOR_RGB(255,255,0); color[5] = COLOR_RGB(0,255,255); color[6] = COLOR_RGB(255,128,128); color[7] = COLOR_RGB(128,255,128); color[8] = COLOR_RGB(128,128,255); color[9] = COLOR_RGB(120,120,0);
  x = xStart;
  for (c = 0; x < width-10; c++) {
    x = c * pixPerMM / xDivs + xStart; 
    if (x < width-10) {
      for (m = x; m < x+4; m++)
        for (n = yStart; n < yStart+16; n++)
          outputImageRGB(m,n) = color[c % xDivs];
    }
  }
  for (m = xStart; m < width-10; m++)
    for (n = yStart; n < yStart+2; n++)
      outputImageRGB(m,n) = color[9];
}


void overlayImageRGB( RGBImage & outputImageRGB, const RGBImage & inputImageRGB, int x0, int y0) {
  int x,y;
  int wdo = outputImageRGB.width();
  int hto = outputImageRGB.height();
  int wd = inputImageRGB.width();
  int ht = inputImageRGB.height();
  for (x = 0; x < wd; x++) {
    for (y = 0; y < ht; y++) {
      if (x+x0 >= 0 && x+x0 < wdo && y+y0 >= 0 && y+y0 < hto)
        outputImageRGB(x + x0, y + y0) = inputImageRGB(x,y);
    }
  }
}


/*
void overlayTextRGB( RGBImage & outputImageRGB, char *text, int x0, int y0) {
  RGBImage 
  int length = strlen(text);
  
  for (i = 0; i < length; i++)  {
    concatImageRGB(
  }
  for (x = 

}
*/

// draws a box with a certain color and line thickness
void drawBoxRGB( RGBImage & inputImageRGB, int x1, int y1, int x2, int y2, int color, int thickness ) {
  int wd = inputImageRGB.width();
  int ht = inputImageRGB.height();
  int t,x,y;
  for (t = 0; t < thickness; t++) {
    for (x = x1; x < x2; x++) {
      if (withinRange(0,wd-1,x)) {
        if (withinRange(0,ht-1,y1+t))
          inputImageRGB(x,y1+t) = color;
        if (withinRange(0,ht-1,y2-t-1))
          inputImageRGB(x,y2-t-1) = color;
      }
    }
    for (y = y1; y < y2; y++) {
      if (withinRange(0,ht-1,y)) {
        if (withinRange(0,wd-1,x1+t))
          inputImageRGB(x1+t,y) = color;
        if (withinRange(0,wd-1,x2-t-1))
          inputImageRGB(x2-t-1,y) = color;
      }
    }
  }
}


void drawLineRGB( RGBImage & inputImageRGB, int x1, int y1, int x2, int y2, int color, int thickness) {
  int half = thickness/2;
  int x,y,m,sign;
  if (abs(x2-x1) > abs(y2-y1)) {
    sign = (x2-x1)/abs(x2-x1);
    for (x = x1; x != x2; x += sign) {
      y = (int) (1.0*(y2-y1)/(x2-x1)*(x-x1)) + y1;
      for (m = -half; m < half; m++) {
        if (inputImageRGB.isInside(x+m,y)) inputImageRGB(x+m,y) = color;
        if (inputImageRGB.isInside(x,y+m)) inputImageRGB(x,y+m) = color;
      }
    }
  } else {
    sign = (y2-y1)/abs(y2-y1);
    for (y = y1; y != y2; y += sign) {
      x = (int) (1.0*(x2-x1)/(y2-y1)*(y-y1)) + x1;
      for (m = -half; m < half; m++)
        if (inputImageRGB.isInside(x+m,y)) inputImageRGB(x+m,y) = color;
        if (inputImageRGB.isInside(x,y+m)) inputImageRGB(x,y+m) = color;
    }
  }

}



void grayscaleDouble( Image<double> & gray, const RGBImage & img )  {
  int i, wd = img.width(), ht = img.height(), npix = wd*ht;
  gray.resize( wd,ht );
  for (i = 0; i < npix; i++ )
    gray[i] = (0 + RED(img[i]) + GREEN(img[i]) + BLUE(img[i])) / 3;
}


//========================================================================
// this is a function that takes an input RGBImage and makes a gray image
void makeGrayFromRGB( Image<unsigned char> & grayOutput, const RGBImage & colorImage )  {
  int width,height,npix,pix,p;
  unsigned char red, green, blue;
  
  width = colorImage.width();
  height = colorImage.height();
  grayOutput.resize( width,height );  // output image has same dimensions as the input color image
  npix = width * height;
  //  visit every pixel in the input image  
  for (p = 0; p < npix; p++) {
      //  pix is of type  int
      pix = colorImage[p];   // fetch the colored pixel from colorImage(x,y)
      red = RED(pix);       // get the RED value
      green = GREEN(pix);   // get the GREEN value
      blue = BLUE(pix);     // get the BLUE value
      // compute the  "black-and-white"  or  "gray value"  or "intensity"  of the pixel
      grayOutput[p] = (red + green + blue) / 3;      // average of the red, green and blue
  }
}    // end of function 


void makeRGBFromGray( RGBImage & colorOutputRGB, Image<unsigned char> & grayImage) {
  int npix,p;
  int width = grayImage.width();
  int height = grayImage.height();
  colorOutputRGB.resize( width,height );  // color output has same dimensions as the input image
  npix = width * height;
  //  visit every pixel in the input image  
  for (p = 0; p < npix; p++) {
    colorOutputRGB[p] = COLOR_RGB(grayImage[p],grayImage[p],grayImage[p]);
  }
}


//  scale == the scale of colorOutputRGB relative to binary1
//         1 = same scale; 2 = twice the dimensions, ...
void makeRGBFromBinary( RGBImage & colorOutputRGB, Image<unsigned char> & binary1, int method, int scale) {
  int i,x,y,pix;
  int black = COLOR_RGB(0,0,0);
  int white = COLOR_RGB(255,255,255);
  int grey = COLOR_RGB(128,128,128);
  int red = COLOR_RGB(255,0,0);
  int width = binary1.width();
  int height = binary1.height();
  int npix = width*height;
  switch (method) {
  case 0:    // method == 0   [0 == white, 1 == black]
    for (x = 0; x < width;  x++) {
      for (y = 0; y < height; y++) {
        if (binary1(x,y) == 1)
          colorOutputRGB(scale * x, scale * y) = black;
        else
          colorOutputRGB(scale * x, scale * y) = white;
      }
    }
    break;
  case 1:  //   method == 1   [0 == black, 1 == original]
    for (x = 0; x < width;  x++) {
      for (y = 0; y < height; y++) {
        if (binary1(x,y) == 0)
          colorOutputRGB(scale * x, scale * y) = black;
      }
    }
    break;
  case 2:  // method == 2   [0 == white, 1 == original]
    for (x = 0; x < width;  x++) {
      for (y = 0; y < height; y++) {
        if (binary1(x,y) == 0)
          colorOutputRGB(scale * x, scale * y) = white;
      }
    }
    break;
  case 3:  // method == 3   [0 == gray, 1 == original]
    for (x = 0; x < width;  x++) {
      for (y = 0; y < height; y++) {
        if (binary1(x,y) == 0)
          colorOutputRGB(scale * x, scale * y) = grey;
      }
    }
    break;
  case 4:  // method == 4   [0 == gray, 1 == original+striped+boundary outline]
    for (x = 1; x < width-2;  x++) {
      for (y = 1; y < height-2; y++) {
        if (binary1(x,y) != binary1(x+1,y) || binary1(x,y) != binary1(x,y+1) || (binary1(x,y) && y%10==0)) {
           colorOutputRGB(scale * x,scale * y-1) = red;
           colorOutputRGB(scale * x-1,scale * y) = colorOutputRGB(scale * x,scale * y) = colorOutputRGB(scale * x+1,scale * y) = red;
           colorOutputRGB(scale * x,scale * y+1) = red;
        }
      }
    }
    break;
  case 5:  // method == 5   [0 == gray, 1 == original+boundary outline]
    for (x = 1; x < width-2;  x++) {
      for (y = 1; y < height-2; y++) {
        if (binary1(x,y) != binary1(x+1,y) || binary1(x,y) != binary1(x,y+1)) {
          int x1 = x*scale, x2 = (x+1)*scale;
          int y1 = y*scale, y2 = (y+1)*scale;
          for (int sx = x1; sx < x2; sx++) {
            for (int sy = y1; sy < y2; sy++) {
              colorOutputRGB(sx,sy) = red;
            }
          }
        }
      }
    }
    break;
  case 6:  // method == 6   [0 == darkened original, 1 == original]
    for (x = 0; x < width;  x++) {
      for (y = 0; y < height; y++) {
        if (binary1(x,y) == 0) {
          pix = colorOutputRGB(scale * x,scale * y);
          colorOutputRGB(scale * x,scale * y) = COLOR_RGB(RED(pix)/2,GREEN(pix)/2,BLUE(pix)/2);
        }
      }
    }
    break;
  }
}



/*
void overlayStringRGB( RGBImage & outputImageRGB, char *inputString, int & xCursor, int & yCursor, int size){

case 0; strcpy( charBits,
"00000"
"00000"
"00000"
"00000"
"00000"
"00000"
"00000"
); break;

  char charBits[100];

switch (c) {
case 0: strcpy( charBits,
"01110"
"10001"
"10001"
"10001"
"10001"
"10001"
"01110"
); break;
case 1: strcpy( charBits,
"00100"
"01100"
"00100"
"00100"
"00100"
"00100"
"01110"
); break;
case 2: strcpy( charBits,
"01110"
"10001"
"00001"
"00110"
"01000"
"10000"
"11111"
); break;
case 3: strcpy( charBits,
"01110"
"10001"
"00001"
"00110"
"00001"
"10001"
"01110"
); break;
case 4: strcpy( charBits,
"10001"
"10001"
"10001"
"11111"
"00001"
"00001"
"00001"
); break;
case 5: strcpy( charBits,
"11111"
"10000"
"10000"
"11110"
"00001"
"00001"
"11110"
); break;
case 6: strcpy( charBits,
"01110"
"10000"
"10000"
"11110"
"10001"
"10001"
"01110"
); break;
case 7: strcpy( charBits,
"11111"
"00001"
"00001"
"00010"
"00100"
"01000"
"10000"
); break;
case 8: strcpy( charBits,
"01110"
"10001"
"10001"
"01110"
"10001"
"10001"
"01110"
); break;
case 9: strcpy( charBits,
"01110"
"10001"
"10001"
"01111"
"00010"
"00100"
"01000"
); break;
}  // switch

}
*/
