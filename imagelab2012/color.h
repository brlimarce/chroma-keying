

#ifndef COLOR_H
#define COLOR_H

#include <math.h>
#include "image.h"

// The L component of LAB and LUV has a minimum of  -16.0
// but L = [-16..0] is a LINEAR function (see FAQ on Color)

#define L_MIN    0.0   
#define L_MAX  100.0   
#define A_MIN  -97.0  
#define A_MAX   88.0  
#define B_MIN -100.0  
#define B_MAX   88.0  

#define U_MIN  -96.0  
#define U_MAX  174.0  
#define V_MIN -134.0  
#define V_MAX  109.0 

enum ILLUMINANTS { WHITE_EQUAL, WHITE_TUNG, WHITE_NOON, WHITE_OVER,
                   WHITE_BBODY };

enum PRIMARIES { PRIM_CIEXYZ, PRIM_NTSC, PRIM_CRT };

void RGBtoXYZ( double   r, double   g, double   b,
              double & X, double & Y, double & Z, int prim, int white);

void XYZtoRGB ( double   X, double   Y, double   Z,
               double & R, double & G, double & B, int prim, int white);

void MakeWhite( double & x0, double & y0, double & z0, int white );

void RGBtoLAB( double   r, double   g, double b,
              double & L, double & A, double & B, int prim, int white);

void RGBtoLUV( double   r, double   g, double   b,
              double & L, double & U, double & V, int prim, int white);

void LABtoRGB( double   L, double  As, double   Bs,
              double & R, double & G, double & B, int prim, int white );

void LUVtoRGB( double   L, double   U, double   V,
              double & R, double & G, double & B, int prim, int white );

// RGBtoYUV() and YUVtoRGB()
// r,g,b,Y are in the range 0..1
// U is in [-0.436,0.436]
// V is in [-0.615,0.615]
void RGBtoYUV( double   r, double   g, double   b,
              double & Y, double & U, double & V);

void YUVtoRGB( double   Y, double   U, double   V,
              double & r, double & g, double & b);

void RGBtoYIQ( double   r, double   g, double   b,
              double & Y, double & I, double & Q);

void YIQtoRGB( double   Y, double   I, double   Q,
              double & r, double & g, double & b);

void YIQtoXYZ( double   Y, double   I , double   Q,
              double & X, double & Y2, double & Z);

void XYZtoYIQ( double   X, double  Y2, double   Z,
              double & Y, double & I, double & Q);

// HSI is Hue, Saturation and Intensity

void RGBtoHSI( double   r, double   g, double   b,
              double & H, double & S, double & I);

void HSItoRGB( double   H, double   S, double   I,
              double & R, double & G, double & B);


// 24Jan1012, second version of dominant color segmenter.  This time, hue is converted to
// unsigned char 0..255 and a threshold applied to intensity 0..255 
// find the dominant hue of the background and binarizes the color image, setting the dominant color to 0 and the foreground to 1 
void colorSegment2(Image<unsigned char> & binary1, const RGBImage & colorImage1);


// find the dominant hue of the background and binarizes the color image, setting the dominant hue to 0 and the foreground to 1 
void colorSegment(Image<unsigned char> & binary1, const RGBImage & colorImage1);

#endif     // COLOR_H











