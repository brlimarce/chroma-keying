/*
   Important notes:

   R,G and B is scaled from [0..255] to [0..100] before applying the
   RGB-to-XYZ matrix transformation.  

 */

#include "color.h"
#include<math.h>
#include "tools.h"
//#include "algo.h"   // min() function
//#include <algorithm.h>  

#define NPRIM  3   // number of PRIMARY definitions
#define NWHITE 5   // number of reference WHITE

// using namespace std;

// The following were copied from http://www.cplusplus.com/reference/algorithm/max/
/*
template <class T> const T& max ( const T& a, const T& b ) {
  return (b<a)?a:b;     // or: return comp(b,a)?a:b; for the comp version
}

template <class T> const T& min ( const T& a, const T& b ) {
  return (b>a)?a:b;     // or: return comp(b,a)?a:b; for the comp version
}
*/
// transformation matrix for converting RGB to XYZ
// for primary = (PRIM_CIEXYZ, PRIM_NTSC, PRIM_CRT)
//    for white = (WHITE_EQUAL, WHITE_TUNG, WHITE_NOON, WHITE_OVER, WHITE_BBODY)

static const double Trans_M[] = {
 0.48803, 0.31076, 0.20121, 0.17596, 0.81320, 0.01084, 0.00000, 0.01021, 0.99280,
 0.75113, 0.27722, 0.06969, 0.27082, 0.72543, 0.00376, 0.00000, 0.00911, 0.34384,
 0.51348, 0.30790, 0.17010, 0.18513, 0.80570, 0.00917, 0.00000, 0.01011, 0.83932,
 0.42450, 0.31876, 0.23775, 0.15305, 0.83414, 0.01281, 0.00000, 0.01047, 1.17307,
 0.41210, 0.32087, 0.21840, 0.14858, 0.83965, 0.01177, 0.00000, 0.01054, 1.07761,

 0.66058, 0.17109, 0.16833, 0.32536, 0.57845, 0.09619, 0.00000, 0.06518, 0.93783,
 0.88793, 0.15754, 0.05258, 0.43734, 0.53262, 0.03004, 0.00000, 0.06001, 0.29293,
 0.67756, 0.17330, 0.14061, 0.33373, 0.58592, 0.08035, 0.00000, 0.06602, 0.78341,
 0.60699, 0.17345, 0.20057, 0.29897, 0.58642, 0.11461, 0.00000, 0.06608, 1.11747,
 0.58932, 0.17898, 0.18307, 0.29026, 0.60513, 0.10461, 0.00000, 0.06818, 1.01996,

 0.55918, 0.28147, 0.15935, 0.30808, 0.61755, 0.07436, 0.02315, 0.15124, 0.82861,
 0.81944, 0.24228, 0.03631, 0.45148, 0.53158, 0.01695, 0.03393, 0.13018, 0.18883,
 0.57916, 0.28280, 0.12952, 0.31909, 0.62047, 0.06044, 0.02398, 0.15195, 0.67350,
 0.49769, 0.28960, 0.19372, 0.27421, 0.63539, 0.09040, 0.02061, 0.15561, 1.00733,
 0.47821, 0.29856, 0.17460, 0.26347, 0.65505, 0.08148, 0.01980, 0.16042, 0.90793
};


// inverse of the above RGBtoXYZ transformation matrices;  
// this is for converting XYZ to RGB
// for primary = (PRIM_CIEXYZ, PRIM_NTSC, PRIM_CRT)
//    for white = (WHITE_EQUAL, WHITE_TUNG, WHITE_NOON, WHITE_OVER, WHITE_BBODY)

static const double Inv_M[] = {
  2.3740, -0.9013, -0.4713, -0.5138,  1.4249,  0.0886,  0.0053, -0.0147,  1.0063,
  1.5425, -0.5856, -0.3062, -0.5759,  1.5973,  0.0993,  0.0153, -0.0423,  2.9057,
  2.2564, -0.8566, -0.4479, -0.5185,  1.4382,  0.0894,  0.0062, -0.0173,  1.1904,
  2.7293, -1.0362, -0.5418, -0.5008,  1.3892,  0.0863,  0.0045, -0.0124,  0.8517,
  2.8114, -1.0674, -0.5581, -0.4976,  1.3800,  0.0858,  0.0049, -0.0135,  0.9271,

  1.7548, -0.4892, -0.2648, -0.9985,  2.0273, -0.0287,  0.0694, -0.1409,  1.0683,
  1.3055, -0.3639, -0.1970, -1.0845,  2.2018, -0.0311,  0.2222, -0.4511,  3.4202,
  1.7108, -0.4769, -0.2581, -0.9858,  2.0015, -0.0283,  0.0831, -0.1687,  1.2789,
  1.9097, -0.5324, -0.2882, -0.9850,  1.9998, -0.0283,  0.0582, -0.1183,  0.8966,
  1.9669, -0.5483, -0.2968, -0.9545,  1.9379, -0.0274,  0.0638, -0.1295,  0.9823,

  2.3427, -0.9789, -0.3627, -1.1869,  2.1517,  0.0352,  0.1512, -0.3654,  1.2106,
  1.5986, -0.6680, -0.2474, -1.3789,  2.4997,  0.0408,  0.6634, -1.6032,  5.3121,
  2.2619, -0.9452, -0.3502, -1.1813,  2.1415,  0.0350,  0.1860, -0.4495,  1.4894,
  2.6322, -1.0999, -0.4075, -1.1536,  2.0913,  0.0342,  0.1244, -0.3006,  0.9958,
  2.7394, -1.1447, -0.4241, -1.1190,  2.0285,  0.0331,  0.1380, -0.3334,  1.1048
};

//  white coordinates in the CIE space;  used by makeWhite()
//    for white = (WHITE_EQUAL, WHITE_TUNG, WHITE_NOON, WHITE_OVER, WHITE_BBODY)

static const double XYZ_White[] = {
 100.00000, 100.00000, 100.30029,  
 109.80392, 100.00000,  35.29412,
  99.14773, 100.00000,  84.94318,
  98.10126, 100.00000, 118.35442,
  95.13678, 100.00000, 108.81460
};




// -------------------------------------
void RGBtoXYZ ( double   r, double   g, double   b,
                double & X, double & Y, double & Z, int prim, int white)  {
   // "white" is the reference white
   int base;
   double r0,g0,b0;

   base = prim * NWHITE * 9 + white * 9;
   r0 = 100.0 * r / 255.0;
   g0 = 100.0 * g / 255.0;    // r, g, b scaled to [0..100]
   b0 = 100.0 * b / 255.0;

   X = Trans_M[ base + 0 ] * r0 + Trans_M[ base + 1 ] * g0 + Trans_M[ base + 2 ] * b0;
   Y = Trans_M[ base + 3 ] * r0 + Trans_M[ base + 4 ] * g0 + Trans_M[ base + 5 ] * b0;
   Z = Trans_M[ base + 6 ] * r0 + Trans_M[ base + 7 ] * g0 + Trans_M[ base + 8 ] * b0;
}


void XYZtoRGB ( double   X, double   Y, double   Z,
               double & R, double & G, double & B, int prim, int white)  {
   int base;
   double r,g,b;
   base = prim * NWHITE * 9 + white * 9;   
   r = Inv_M[ base + 0 ] * X + Inv_M[ base + 1 ] * Y + Inv_M[ base + 2 ] * Z;
   g = Inv_M[ base + 3 ] * X + Inv_M[ base + 4 ] * Y + Inv_M[ base + 5 ] * Z;
   b = Inv_M[ base + 6 ] * X + Inv_M[ base + 7 ] * Y + Inv_M[ base + 8 ] * Z;

   R = r / 100.0 * 255.0;
   G = g / 100.0 * 255.0;    // r, g, b scaled from [0..100] to [0..255]
   B = b / 100.0 * 255.0;
   R = max( R,0.0 );   R = min( R,255.0 );
   G = max( G,0.0 );   G = min( G,255.0 );
   B = max( B,0.0 );   B = min( B,255.0 );
}


void makeWhite( double & x0, double & y0, double & z0, int white ) {
   int base;
   base = white * 3;
   x0 = XYZ_White[ base + 0];
   y0 = XYZ_White[ base + 1];
   z0 = XYZ_White[ base + 2];
}


void RGBtoLAB ( double   r, double   g, double b,
               double & L, double & A, double & B, int prim, int white)  {
   double x,y,z, xn,yn,zn, third, xxn,yyn,zzn,fx,fy,fz;
   double constraint = 0.008856;
   double const16    = 16.0/116.0;

   RGBtoXYZ( r,g,b, x,y,z, prim, white );
   makeWhite( xn, yn, zn,  white );

   xxn = x / xn;  yyn = y / yn;  zzn = z / zn;

   third = 1.0 / 3.0;
   
   fx = (xxn > constraint) ? pow(xxn,third) : 7.787 * xxn + const16;
   fy = (yyn > constraint) ? pow(yyn,third) : 7.787 * yyn + const16;
   fz = (zzn > constraint) ? pow(zzn,third) : 7.787 * zzn + const16;
  
   L = (yyn > constraint) ? 116.0 * pow(yyn,third) - 16.0 : 903.3 * yyn;
   A = 500 * (fx - fy);
   B = 200 * (fy - fz);
}


// ---------------------------------
void RGBtoLUV ( double   r, double   g, double   b,
               double & L, double & U, double & V, int prim, int white)  {
   double x,y,z, xn,yn,zn, up,vp,un,vn, Ls,Us,Vs, yyn;
   double constraint = 0.008856;

   RGBtoXYZ(  r,g,b, x,y,z, prim, white );
   makeWhite( xn, yn, zn,   white );
   yyn = y / yn;
   up = 4 * x  / (x  + 15 * y  + 3 * z );
   vp = 9 * y  / (x  + 15 * y  + 3 * z );
   un = 4 * xn / (xn + 15 * yn + 3 * zn);
   vn = 9 * yn / (xn + 15 * yn + 3 * zn);

   Ls = (yyn > constraint) ? 116.0 * pow(yyn,1.0/3.0) - 16.0 : 903.3 * yyn;
   Us = 13 * Ls * (up - un);
   Vs = 13 * Ls * (vp - vn);

   L = Ls;   U = Us;  V = Vs;
}


void LABtoRGB( double   L, double  As, double   Bs,
              double & R, double & G, double & B, int prim, int white )  {
   double xn,yn,zn;   // reference white
   double x,y,z,yfrac;

   makeWhite( xn,yn,zn, white );
   y = pow( (L+16)/116 , 3.0) * yn;
   yfrac = pow( y/yn, 1.0/3.0);   
   x = pow( As/500.0 + yfrac , 3.0) * xn;
   z = pow( yfrac - Bs/200.0 , 3.0) * zn;
   x = max( x, xn * 0.01 );
   y = max( y, yn * 0.01 );
   z = max( z, zn * 0.01 );

   XYZtoRGB( x,y,z,  R,G,B, prim,white );
}


void LUVtoRGB( double   L, double   U, double   V,
              double & R, double & G, double & B, int prim, int white )  {
   double xn,yn,zn;   // reference white
   double un,vn,uc,vc;
   double x,y,z;

   makeWhite( xn,yn,zn, white );
   un = 4.0 * xn / (xn + 15.0 * yn + 3.0 * zn);
   vn = 9.0 * yn / (xn + 15.0 * yn + 3.0 * zn);
   uc = U / (13.0 * L) + un;
   vc = V / (13.0 * L) + vn;

   y = pow( (L+16.0)/116.0 , 3.0) * yn;
   x = 9.0 * y * uc / (4.0 * vc);
   z = (4.0 * x - uc * (x + 15.0 * y)) / (3.0 * uc);
   x = max( x, xn * 0.01 );
   y = max( y, yn * 0.01 );
   z = max( z, zn * 0.01 );

   XYZtoRGB( x,y,z,  R,G,B, prim,white );
}


// ---- where did this come from? -- SPIE 3312, p 388
void RGBtoHSV( double   r, double   g, double   b,
              double & H, double & S, double & V)  {
   double max2,min2;
   max2 = max( r, max(g,b) );
   min2 = min( r, min(g,b) );   

   if (max2 == 0)   { H = 0.0;  S = 0.0;  }
   else  {
      S = 1.0 * (max2-min2) / max2;
      if (max2 == r)   H = 1.0 * (g - b) / (max2 - min2)          ;
      if (max2 == g)   H = 1.0 * (b - r) / (max2 - min2) + 1.0/3.0;
      if (max2 == b)   H = 1.0 * (r - g) / (max2 - min2) + 2.0/3.0;
   }

   V = 1.0 * max2 / 255.0;
}


// RGBtoYUV() and YUVtoRGB()
// r,g,b,Y are in the range 0..1
// U is in [-0.436,0.436]
// V is in [-0.615,0.615]
void RGBtoYUV( double   r, double   g, double   b,
              double & Y, double & U, double & V)  {
   Y = 0.2989 * r + 0.5866 * g + 0.1144 * b;
   U = -0.147 * r - 0.289 * g + 0.436 * b;
   V = 0.615 * r - 0.515 * g - 0.100 * b;
}

void YUVtoRGB( double   Y, double   U, double   V,
              double & r, double & g, double & b)  {
   r = 1.0 * Y - 0.000039457070707 * U + 1.139827967171717 * V;
   g = 1.0 * Y - 0.394610164141414 * U - 0.580500315656566 * V;
   b = 1.0 * Y + 2.031999684343434 * U - 0.000481376262626 * V;
}

// -------------------------
void RGBtoYIQ( double   r, double   g, double   b,
              double & Y, double & I, double & Q)  {
   Y = 0.2989 * r + 0.5866 * g + 0.1144 * b;
   I = 0.5959 * r - 0.2741 * g - 0.3218 * b;
   Q = 0.2113 * r - 0.5227 * g + 0.3113 * b;
}


void YIQtoRGB( double   Y, double   I, double   Q,
              double & r, double & g, double & b)  {
   r = 1.0 * Y + 0.9562 * I + 0.6210 * Q;
   g = 1.0 * Y - 0.2717 * I - 0.6485 * Q;
   b = 1.0 * Y - 1.1053 * I + 1.7020 * Q;
}


// ---- NOTE: the next two conversions are said to be independent of the -------
// ---------- alignment white-------------------------------
void YIQtoXYZ( double   Y, double   I , double   Q,
              double & X, double & Y2, double & Z)  {
   X  = 0.9810 * Y + 0.3116 * I + 0.6059 * Q;
   Y2 = 1.0000 * Y - 0.0001 * I + 0.0004 * Q;
   Z  = 1.1836 * Y - 1.2531 * I + 1.8591 * Q;
}


void XYZtoYIQ( double   X, double  Y2, double   Z,
              double & Y, double & I, double & Q)  {
   Y = -0.0003 * X + 1.0004 * Y2 - 0.0002 * Z;
   I =  1.3893 * X - 0.8274 * Y2 - 0.4525 * Z;
   Q =  0.9365 * X - 1.1946 * Y2 + 0.2330 * Z;
}


void RGBtoCMY(int r, int g, int b,
             double & C, double & M, double & Y)  {
   C = 1.0 - r;
   M = 1.0 - g;
   Y = 1.0 - b;
}


// ------------------ RGB <--> HSI ------------------------

void RGBtoHSI( double   r, double   g, double   b,
              double & H, double & S, double & I) {
// from Digital Image Processing (1st ed.) by Gonzalez and Woods p 299

   double h,s,i,sumrgb;
	// input r,g,b are in the range [0..255]
   r = r / 255.0;        
   g = g / 255.0;
   b = b / 255.0;

   sumrgb = r + g + b;
   // Intensity:  range is [0..1]
   i = 0.00001 + sumrgb / 3.0;

   // Hue:  this is an angle [0..2*PI]
   // when r == g == b, the denominator becomes zero
   // so we added a very small number to prevent division by zero
   h = acos ( 0.5 * ((r-g) + (r-b))
               / (0.00001 + sqrt((r-g)*(r-g) + (r-b)*(g-b))) );  
   if (b/i > g/i)  h = 2*3.14159 - h;

   // saturation:  range is [0..1]
   min(1,2);
   s = 1.0 - 3.0 * min(r,min(g,b)) / (0.00001 + r+g+b); 

   H = h;   S = s;   I = i;
}


void HSItoRGB( double   H, double   S, double   I,
              double & R, double & G, double & B)  {
// from Digital Image Processing (1st edn) by Gonzalez and Woods p 299-300
   double r,g,b;
   double pi = 3.14159265;  

   if (S < 0.05) {
	r = g = b = 1.0/3.0;
   }
   else if (H <= 2.0 * pi / 3.0)  {
      b = (1.0 - S) / 3.0;
      r = (1.0 + S * cos(H) / (0.00001 + cos(pi/3.0 - H))) / 3.0;
      g = 1 - (r + b);
   }
   else if (H <= 4.0*pi/3.0)  {
      H = H - 2.0*pi/3.0;
      r = (1.0 - S) / 3.0;
      g = (1.0 + S * cos(H) / (0.00001 + cos(pi/3.0 - H))) / 3.0;
      b = 1 - (r + g);
   }
   else {
      H = H - 4.0*pi/3.0;
      g = (1.0 - S) / 3.0;
      b = (1.0 + S * cos(H) / (0.00001 + cos(pi/3.0 - H))) / 3.0;
      r = 1 - (g + b);
   }
   // at this point, r/g/b are in the range [0..1]

   R =  min( 254.0, max(0.0, 3.0 * I * r * 254));  
   G =  min( 254.0, max(0.0, 3.0 * I * g * 254)); 
   B =  min( 254.0, max(0.0, 3.0 * I * b * 254));
}


// 24Jan1012, second version of dominant color segmenter.  This time, hue is converted to
// unsigned char 0..255 and a threshold applied to intensity 0..255 
// find the dominant hue of the background and binarizes the color image, setting the dominant color to 0 and the foreground to 1 
void colorSegment2(Image<unsigned char> & binary1, const RGBImage & colorImage1) {
  Image<unsigned char> hueImage, intensityImage;
  Image<int> hueHistogram;   // a 1-D array
  int x, y, width, height, pix;
  int numHueBins = 255;
  int b, i, windowWidth = 6;
  float dHue = 2.0 * 3.14159265 / numHueBins;
  double hue, saturation, intensity;
  int sum, maxSum, maxCenter;
  int hueRadius = 8;
  int intensityThreshold = 38;  // estimated from the dark blue shaded region in blueenvelope-elevate9mm-lexscan.jpg

  width = colorImage1.width();
  height = colorImage1.height();

  hueImage.resize(width,height);   // hue 0..2pi is scaled to 0..255
  intensityImage.resize(width,height);   // intensity is scaled to 0..255

  hueHistogram.resize( numHueBins,1 );
  hueHistogram.setAll( 0 );
  for (x = 0; x < width; x++) {
    for (y = 0; y < height; y++) {
      pix = colorImage1(x,y);
      RGBtoHSI(RED(pix),GREEN(pix),BLUE(pix),hue,saturation,intensity);
      hueHistogram[(int)(hue/dHue)]++;
      hueImage(x,y) = (int)(hue/dHue);
      intensityImage(x,y) = (unsigned char)(255*intensity);

      // if (x%10==0 && y%10==0)  printf("%d ",(int)(hue/dHue));
    }
  }

  // find the maximum window sum in the hue histogram
  // this will be considered the mode, or the dominant hue in the image
  // Future work:  The mode can be CENTERED on its current location in the histogram.
  //               This will make the computation of the dominant hue more accurate.
  maxSum = 0;
  maxCenter = windowWidth / 2;
  for (b = 0; b < numHueBins-windowWidth; b++) {

    for (sum = 0, i = 0; i < windowWidth; i++)
      sum += hueHistogram[b+i];
    if (sum > maxSum) {
      maxSum = sum;
      maxCenter = b + windowWidth / 2;
    }
  }

  binary1.resize( width,height );
  binary1.setAll( 0 );
  for (x = 0; x < width; x++) {
    for (y = 0; y < height; y++) {
      if (abs(hueImage(x,y) - maxCenter) > hueRadius ||
          intensityImage(x,y) < intensityThreshold)
        binary1(x,y) = 1;
    }
  }
}


// First version of dominant color segmenter, using only the Hue component
//
// find the dominant hue of the background and binarizes the color image, setting the dominant hue to 0 and the foreground to 1 
void colorSegment(Image<unsigned char> & binary1, const RGBImage & colorImage1) {
  Image<int> hueImage;
  Image<int> hueHistogram;
  int x, y, width, height, pix;
  int numHueBins = 360;
  int b, i, windowWidth = 7;
  float dHue = 2.0 * 3.14159 / numHueBins;
  double hue, saturation, intensity;
  int sum, maxSum, maxCenter;
  int hueRadius = 10;

  width = colorImage1.width();
  height = colorImage1.height();

  hueImage.resize(width,height);
  hueHistogram.resize( numHueBins,1 );
  hueHistogram.setAll( 0 );
  for (x = 0; x < width; x++) {
    for (y = 0; y < height; y++) {
      pix = colorImage1(x,y);
      RGBtoHSI(RED(pix),GREEN(pix),BLUE(pix),hue,saturation,intensity);
      hueHistogram[(int)(hue/dHue)]++;
      hueImage(x,y) = (int)(hue/dHue);
      // if (x%10==0 && y%10==0)  printf("%d ",(int)(hue/dHue));
    }
  }

  // find the maximum window sum in the hue histogram
  // this will be considered the mode, or the dominant hue in the image
  // Future work:  The mode can be CENTERED on its current location in the histogram.
  //               This will make the computation of the dominant hue more accurate.
  maxSum = 0;
  maxCenter = windowWidth / 2;
  for (b = 0; b < numHueBins-windowWidth; b++) {

    for (sum = 0, i = 0; i < windowWidth; i++)
      sum += hueHistogram[b+i];
    if (sum > maxSum) {
      maxSum = sum;
      maxCenter = b + windowWidth / 2;
    }
  }

  binary1.resize( width,height );
  binary1.setAll( 0 );
  for (x = 0; x < width; x++) {
    for (y = 0; y < height; y++) {
      if (abs(hueImage(x,y) - maxCenter) > hueRadius)
        binary1(x,y) = 1;
    }
  }
}


