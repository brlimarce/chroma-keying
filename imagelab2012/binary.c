
#include "image.h"
#include "binary.h"
#include "math.h"
#include "stdlib.h"


// area of the binary image
int binaryArea( const Image<unsigned char> & img )  {
  int npix = img.width()*img.height();
  int i;
  int area = 0;
  for (i = 0; i < npix; i++)
    area += img[i];
  return area;
}


void binaryGetFirstPixel( int & xFirst, int & yFirst, const Image<unsigned char> & img)  {
  int y, x, stop = 0, w = img.width(), h = img.height();
/*
  y = 0;
  while (y < h && img(x,y) == 0) {
    x = 0;
    while (x < w && img(x,y) == 0) {
      x++;
    }
    y++;
  }
*/
  for (y = 0; y < h && stop==0; y++)
    for (x = 0; x < w && stop==0; x++)
      if (img(x,y)==1)
        stop = 1;

  if (img(x-1,y-1) != 1) {
    printf("Error: binaryGetFirstPixel: Did not find a 1-pixel.\n");
  }
  else {
    xFirst = x-1;
    yFirst = y-1;
  }
}

// traces the boundary of binary image img, starting from (x,y)
// (x,y) must be a foreground pixel (one) on the boundary of the object
// the coordinates of the perimeter is colored in boundaryRGB

int binaryFollowBoundary(RGBImage & boundaryRGB, float & perimeter, const Image<unsigned char> & img, int startX, int startY)  {
  int xn,yn,xtemp,ytemp,nb,xb,yb,stop;
  int width = img.width();
  int height = img.height();
  
  if (img(startX,startY) == 0) {
    printf("Error:  The starting point is a background pixel!\n");
    return 0;
  }

  Image<int> eightN;  // eight neighbor
  eightN.resize(8,2);
  eightN(0,0) = -1;  eightN(0,1) = -1;
  eightN(1,0) = 0;  eightN(1,1) = -1;
  eightN(2,0) = 1;  eightN(2,1) = -1;
  eightN(3,0) = 1;  eightN(3,1) = 0;
  eightN(4,0) = 1;  eightN(4,1) = 1;
  eightN(5,0) = 0;  eightN(5,1) = 1;
  eightN(6,0) = -1;  eightN(6,1) = 1;
  eightN(7,0) = -1;  eightN(7,1) = 0;
  
  boundaryRGB.resize(width,height);
  boundaryRGB.setAll(COLOR_RGB(255,255,255));
  
  for (nb = 0, stop = 0; !stop && nb < 8; nb++)  {
    xn = startX + eightN(nb,0);   //  x-neighbor
    yn = startY + eightN(nb,1);   // y-neighbor
    if (xn < 0 || xn >= width || yn < 0 || yn >= height || img(xn,yn) == 0)
        stop = 1;
  }
  // nb now points to the index of the next neighbor

  if (nb == 8) {
    printf("Error:  The starting point is not on an object boundary!\n");
    return 0;
  }
  nb = (nb + 8 - 1) % 8;  // move back counter-clockwise one step
  
  xb = startX;
  yb = startY;
  perimeter = 0;
  // while not yet circled and arrived in starting position
  stop = 0;
  while (!stop) {
//    printf("nb = %d, xn = %d, yn = %d, xb = %d, yb = %d \n",nb,xn,yn,xb,yb);
    if (xn < 0 || xn >= width || yn < 0 || yn >= height || img(xn,yn) == 0) {
      nb = (nb + 1) % 8;
      xn = xb + eightN(nb,0);   //  x-neighbor
      yn = yb + eightN(nb,1);   // y-neighbor
    }
    else
    if (img(xn,yn) == 1) {
      if (xn == startX && yn == startY) 
        stop = 1;

      boundaryRGB(xn,yn) = COLOR_RGB(255,0,0); // color the found boundary point
      perimeter += sqrt((xb-xn)*(xb-xn)+(yb-yn)*(yb-yn));
      xb = xn;
      yb = yn;
      nb = (nb + 4 + 1) % 8;   // exchange position and advance by one step clockwise
      xn = xb + eightN(nb,0);   //  x-neighbor
      yn = yb + eightN(nb,1);   // y-neighbor
    }
  }
  boundaryRGB(startX,startY) = COLOR_RGB(255,0,0);
  return 1;
}


Image<unsigned char> binaryComplement( const Image<unsigned char> & img )  {
   Image<unsigned char> outimg;
   int h = img.height(), w = img.width(), x,y;
   outimg.resize( w,h );
   outimg.setAll( 0 );
   for (x = 0; x < w; x++) 
   for (y = 0; y < h; y++) {
     outimg( x,y ) = img(x,y) ? 0 : 1;
   }
   return outimg;
}

void binaryMedianFilter( Image<unsigned char> & outputImage, const Image<unsigned char> & inputImage, int filterWidth, int filterHeight)  {
  int h = inputImage.height(),
      w = inputImage.width();
  int x,x2,y,sum;
  int area = filterWidth * filterHeight;
  int halfArea = area / 2;
  Image<int> horizSum;
  outputImage.resize( w,h );
  outputImage.setAll( 0 );
  horizSum.resize(h,1);
  for (x = 0; x < w-filterWidth; x++) {   // compute vertical sums
    if (x == 0) {
      horizSum.setAll(0);
      for (y = 0; y < h; y++) {    // compute first column horizontal sums
        for (x2 = 0; x2 < filterWidth; x2++)  
          horizSum[y] += inputImage(x2,y);
      }
    } else {
    for (y = 0; y < h; y++)
      horizSum[y] = horizSum[y] - inputImage(x-1,y) + inputImage(x+filterWidth-1,y);
    }
    for (sum = 0, y = 0; y < filterHeight; y++)   // the top window 
      sum += horizSum[y];
    outputImage(x + filterWidth/2, filterHeight/2) = (sum > halfArea ? 1 : 0);
    for (y = 1; y < h-filterHeight; y++) {    // window slides down
      sum = sum - horizSum[y-1] + horizSum[y+filterWidth-1];
      outputImage(x + filterWidth/2,y + filterWidth/2) = (sum > halfArea ? 1 : 0);
    }
  }
}




/*
this is the old one

void binaryMedianFilter( Image<unsigned char> & outputImage, const Image<unsigned char> & inputImage, int filterWidth)  {
  int h = inputImage.height(),
      w = inputImage.width();
  int x,y;
  int sum;
  int halfArea = filterWidth * filterWidth / 2;
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
    outputImage(x + filterWidth/2, filterWidth/2) = (sum > halfArea ? 1 : 0);
    for (y = 1; y < h-filterWidth; y++) {
      sum = sum - horizSum(x,y-1) + horizSum(x,y+filterWidth-1);
      outputImage(x + filterWidth/2,y + filterWidth/2) = (sum > halfArea ? 1 : 0);
    }
  }
}
*/



void binarizeGrayImage( Image<unsigned char> & outputImage, const Image<unsigned char> & 
inputImage, int method, float parameter1, float parameter2)  {
  int width,height,p,npix, threshold;
  // method 1 = simple threshold:  background is brighter than the foreground
  if (method == 1) {  
    threshold = (int) parameter1;
    width = inputImage.width();
    height = inputImage.height();
    npix = width * height;
    outputImage.resize(width,height);
    for (p = 0; p < npix; p++) {
      if (inputImage[p] < threshold)
        outputImage[p] = 1;
      else 
        outputImage[p] = 0;
    }
  }
}


void binaryCenterOfMass( int & xc, int & yc, const Image<unsigned char> & binaryImage ) {
  int sumx,sumy,x,y,n;
  int width = binaryImage.width();
  int height = binaryImage.height();
  sumx = sumy = n = 0;
  xc = yc = 0;
  for (y = 0; y < height; y++) {
    for (x = 0; x < width; x++) {
      if (binaryImage(x,y)) {
        sumx += x;
        sumy += y;
        n++;
      }
    }
  }
  if (n > 0) {
    xc = sumx / n;
    yc = sumy / n;
  }
}

// -----------------------------------------------------------------------
//  Return the binary img eroded by structuring image strucElem 
//  with origins originX and originY
// -----------------------------------------------------------------------
void binaryErosion(Image<unsigned char> & outputImage, const Image<unsigned char> & img, 
                     const Image<unsigned char> & strucElem, int originX, int originY ) {
   int h = img.height(),
       w = img.width(),
       sh = strucElem.height(),
       sw = strucElem.width(),
       x,y,m,n;
   unsigned char match;

   outputImage.resize( w,h );
   outputImage.setAll( 0 );
   for (x = 0; x < w - sw + 1; x++) 
   for (y = 0; y < h - sh + 1; y++) {
      match = 1;
      for (m = 0; m < sw && match; m++)
      for (n = 0; n < sh && match; n++)
         if (strucElem(m,n) && !img(x+m,y+n))  match = 0;
      outputImage( x+originX, y+originY ) = match;
   }
}



// -----------------------------------------------------------------------
//  Return the binary img dilated by structuring image Struc with origins
// -----------------------------------------------------------------------
void binaryDilation(Image<unsigned char> & outputImage,  const Image<unsigned char> & img, 
                      const Image<unsigned char> & strucElem, int originX, int originY ) {
   int h = img.height(),
       w = img.width(),
       sh = strucElem.height(),
       sw = strucElem.width(),
       x,y,m,n;

   outputImage.resize( w,h );
   outputImage.setAll( 0 );
   for (x = 0; x < w - sw + 1; x++) 
   for (y = 0; y < h - sh + 1; y++) {
      if (img(x+originX,y+originY)) {
        for (m = 0; m < sw; m++)
        for (n = 0; n < sh; n++)
          if (strucElem(m,n))  outputImage( x+m, y+n ) = 1;
      }
   }
}


// -----------------------------------------------------------------------
// instead of perfect fitting, binaryLooseErosion applies a statistical proportion "proportionMatch" (ex. 95%) to test whether the structuring element fits in the current location
// -----------------------------------------------------------------------
void binaryLooseErosion(Image<unsigned char> & outputImage, const Image<unsigned char> & inputImage, const Image<unsigned char> & strucElem, int originX, int originY, float proportionMatch )  {
   int h = inputImage.height(), w = inputImage.width(),
       sh = strucElem.height(), sw = strucElem.width(),
       x,y,m,n;
   int numEmpty;
   int strucArea;  // area of structuring element
   int maxEmpty;
   unsigned char match;

   for (x = 0, strucArea = 0; x < sh*sw; x++)  strucArea += strucElem[x];  // area of strucElem
   maxEmpty = (int)((1.0-proportionMatch)*strucArea);
   outputImage.resize( w,h );
   outputImage.setAll( 0 );
   for (x = 0; x < w - sw - 2; x++) {
     for (y = 0; y < h - sh - 2; y++) {
       match = 1;
       numEmpty = 0;
       for (m = 0; m < sw && match; m++) {
         for (n = 0; n < sh && match; n++) {
           if (strucElem(m,n) && !inputImage(x+m,y+n))  numEmpty++;
           if (numEmpty >= maxEmpty)  match = 0;
         }
       }
       outputImage( x+originX, y+originY ) = match;
     }
   }
}


// -----------------------------------------------------------------------
//  - Make a binary image with a particular shape.  This is useful for making structuring elements.
//  - shapeType = 0 for square, 1 for circle
//  - For making structuring elements, "width" should be an odd number.
// -----------------------------------------------------------------------
void binaryMakeShape(Image<unsigned char> & outputImage, int shapeType, int width)  {
  float radius = width / 2.0;
  int x,y;
  float xe,ye;
  outputImage.resize( width,width );
  outputImage.setAll( 0 );
  switch (shapeType) {
  case 0:    // square
    outputImage.setAll(1);
    break;
  case 1:    // circle
    for (x = 0; x < width; x++) {
      for (y = 0; y < width; y++) {
        xe = x - radius;
        ye = y - radius;
        if ((xe*xe + ye*ye) <= radius*radius)
          outputImage(x,y) = 1;
      }
    }
    break;
  }
  // print the shape
/*
  printf("\n");
  for (y = 0; y < width; y++) {
    for (x = 0; x < width; x++) {
      printf("%d",outputImage(x,y));
    }
    printf("\n");
  }
*/
}



// given the binary image "inputImage", compute the two eigenvalues and eigenvectors from the covariance matrix
void eigenvectors2D( float & ev1, float & ex1, float & ey1, float & ev2, float & ex2, float & ey2, Image<unsigned char> & inputImage) {
  int width = inputImage.width();
  int height = inputImage.height();
  int sumx, sumy, sumxx, sumyy, sumxy, x, y, n;
  float ssx, ssy, ssxy, common, e1, e2;
  float a, b, c, d;  // elements of the 2x2 matrix A

  sumx = sumy = sumxx = sumyy = sumxy = 0;
  n = 0;
  for (y = 0; y < height; y++) {
    for (x = 0; x < width; x++) {
      if (inputImage(x,y)) {
        n++;
        sumx += x;
        sumy += y;
        sumxx += x*x;
        sumyy += y*y;
        sumxy += x*y;
      }
    }
  }
  ssx = (1.0*sumxx - 1.0*sumx*sumx/n)/n;
  ssy = (1.0*sumyy - 1.0*sumy*sumy/n)/n;
  ssxy = (1.0*sumxy - 1.0*sumx*sumy/n)/n;

  a = ssx;
  b = ssxy;
  c = ssxy;
  d = ssy;
  // eigenvalue computation found in   http://en.wikipedia.org/wiki/Eigenvalue_algorithm
  common = sqrt(4.0*b*c + (a-d)*(a-d))/2.0;
  ev1 = (a+d)/2.0 + common;   // first eigenvalue (ev1) and a sample eigenvector (ex1,ey1)
  ex1 = 1.0;
  ey1 = (ev1 - a + c) * ex1 / (ev1 + b - d);
  ev2 = (a+d)/2.0 - common;   // second eigenvalue (ev2) and a sample eigenvector (ex2,ey2)
  ex2 = 1.0;
  ey2 = (ev2 - a + c) * ex2 / (ev2 + b - d);
}


// return the two points on the binary image that are farthest from each other
void farthestPairOfPoints(int & x1max, int & y1max, int & x2max, int & y2max,  int & length, Image<unsigned char> & binary) {

  int x1,y1,x2,y2;
  int p1,p2,x,y;
  int width = binary.width();
  int height = binary.height();
  Image<int> pointList;
  int nPoints;
  int dist2,maxDist;

  nPoints = 0;
  pointList.resize(width*2+height*2, 2);

  for (x = 0; x < width; x++) {
    y1 = 0;
    y2 = height-1;
    while(y1 < y2 && binary(x,y1) != 1)
      y1++;
    while(y1 < y2 && binary(x,y2) != 1)
      y2--;
    if (binary(x,y1) == 1) {   // this column has a 1
      pointList(nPoints,0) = x;
      pointList(nPoints,1) = y1;
      nPoints++;
      pointList(nPoints,0) = x;
      pointList(nPoints,1) = y2;
      nPoints++;
    }
  }

  maxDist = 0;
  for (p1 = 0; p1 < nPoints-1; p1++)  {
    x1 = pointList(p1,0);    
    y1 = pointList(p1,1);    
    for (p2 = p1+1; p2 < nPoints; p2++)  {
      x2 = pointList(p2,0);    
      y2 = pointList(p2,1);    
      dist2 = (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2);
      if (dist2 > maxDist) {
         x1max = x1;
         y1max = y1;
         x2max = x2;
         y2max = y2;
         maxDist = dist2;
      }
    }
  }
  length = (int) sqrt(maxDist);
}

// -----------------------------------------------------------------------
//      Methods of class ConnectedComponents
// -----------------------------------------------------------------------

// -----------------------------------------------------------------------
// returns the number of connected components (numComponents) in binary;
// tagImage( i,j ) becomes the nametag == [0..numComponents)
// uses breadth-first-search/region-growing
// bbox(tag,0..3) becomes the bounding box (x,y,w,h) for CC tag
// -----------------------------------------------------------------------
int ConnectedComponents::analyzeBinary( const Image<unsigned char> & binary, 
                                        int connectType ) {
   Image<int> queue, connect;   // Image<> class is used as a data structure
   int numConnect, qLimit = 3000, 
       w,h, i,x,y,cx,cy,p,npix,c,s,startQ,endQ,currentEndQ,tag;

   if (connectType == FOUR_CONNECTED) {
     connect.resize(2,4); 
     connect[0] =  0;  connect[1] = -1;
     connect[2] = -1;  connect[3] =  0;   
     connect[4] =  1;  connect[5] =  0;
     connect[6] =  0;  connect[7] =  1;
     numConnect = 4;
   }
   else {     // Default:  EIGHT_CONNECTED
     connect.resize(2,8);
     connect[0] = -1;  connect[1] = -1;
     connect[2] = -1;  connect[3] =  0;
     connect[4] = -1;  connect[5] =  1;
     connect[6] =  0;  connect[7] = -1;
     connect[8] =  0;  connect[9] =  1;
     connect[10] = 1;  connect[11] = -1;
     connect[12] = 1;  connect[13] = 0;
     connect[14] = 1;  connect[15] = 1;
     numConnect = 8;
   }

   queue.resize( qLimit,1 );
   w = binary.width();
   h = binary.height();
   mTagImage.resize( w,h );
   mTagImage.setAll( -1 );
   npix = w * h;
   mNumComponents = 0;
   for ( p = 0; p < npix; p++) 
   
   // find every unlabelled  1-pixel  p
   if (binary[p] && mTagImage[p] == -1)  {
      tag = mNumComponents++;
      mTagImage[p] = tag;
      queue[0] = p;   // seed which we will grow
      startQ = 0;
      endQ = 1;
      while (startQ != endQ) {      
        // enqueue all the connected "children" and color them
        currentEndQ = endQ;
         for (s = startQ; s != currentEndQ; s = (s+1) % qLimit) {
           y = queue[s] / w;  x = queue[s] % w;  
           for (c = 0; c < numConnect; c++) {
             cx = x + connect(0,c);  cy = y + connect(1,c);
             if (0 <= cx && cx < w && 0 <= cy && cy < h && 
               binary(cx,cy) && mTagImage( cx,cy ) == -1) {
               mTagImage( cx,cy ) = tag;
               queue[ endQ ] = cy * w + cx;
               endQ = (endQ+1) % qLimit;
             }
           }
         }

         // dequeue all the parents
         startQ = currentEndQ;
      }
   }

   // compute the array of bounding boxes, store them in Bbox
   mBbox.resize( mNumComponents,4 );
   mBbox.setAll( -1 );
   mCount.resize( mNumComponents,2 );
   mCount.setAll( 0 );
   for (i = 0; i < mNumComponents; i++)   
     mCount(i,0) = i;
   for (x = 0; x < w; x++)  {
   for (y = 0; y < h; y++)  {
     if ((tag = mTagImage(x,y)) != -1) { 
       if (mBbox(tag,0) == -1) {   // new box
         mBbox(tag,0) = x;
         mBbox(tag,1) = y;
         mBbox(tag,2) = x;  // max x
         mBbox(tag,3) = y;  // max y
       }
       else {
         if (x < mBbox(tag,0))
           mBbox(tag,0) = x;
         else if (mBbox(tag,2) < x)  
           mBbox(tag,2) = x;
         if (y < mBbox(tag,1))  
           mBbox(tag,1) = y;
         else if (mBbox(tag,3) < y)  
           mBbox(tag,3) = y;
       }

       mCount(tag,1)++;
     }
   }
   }
   
   for (tag = 0; tag < mNumComponents; tag++)   {
     mBbox(tag,2) = mBbox(tag,2) - mBbox(tag,0) + 1;  // max x is changed to bbox width
     mBbox(tag,3) = mBbox(tag,3) - mBbox(tag,1) + 1;  // max y is changed to bbox height
   }
   
   return mNumComponents;
}


// return the binary image of the n-th component
Image<unsigned char> ConnectedComponents::getComponentBinary( int n ) {
   if (n >= getNumComponents() || n < 0) {
     printf("ConnectedComponents::getComponentBinary : n(%d) is not in range 0..%d\n",
	         n,getNumComponents()-1);
//      cout << "ConnectedComponents::getComponentBinary : n(" << n <<
//      ") is not in range 0.." << getNumComponents() << "-1\n";
      exit(1);
   }

   Image<unsigned char> outimg;
   int xstart = mBbox(n,0), 
       ystart = mBbox(n,1);
   int w = mBbox(n,2),
       h = mBbox(n,3);
   int x,y;
   outimg.resize( w,h );
   outimg.setAll( 0 );

   for (x = 0; x < w; x++)  for (y = 0; y < h; y++)
      if (mTagImage(xstart+x,ystart+y) == n)  outimg(x,y) = 1;
   return outimg;
}


// return a multi-random-colored image of all connected components
RGBImage ConnectedComponents::randomColors() {
   if (getNumComponents() == 0) {
     printf("ConnectedComponents::randomColors : Cannot create connected component color image (mNumComponents == 0)\n");
//      cout << "ConnectedComponents::randomColors : Cannot create "
//            "connected component color image (mNumComponents == 0)\n";
      exit(1);
   }

   RGBImage    outimg;
   Image<unsigned char> colorMap;
   int         c,n,h,w,i,npix,tag;

   colorMap.resize( n = getNumComponents(),3 );
   for (c = 0; c < n; c++) {
      colorMap( c,0 ) = rand() % 200 + 25;
      colorMap( c,1 ) = rand() % 200 + 25;
      colorMap( c,2 ) = rand() % 200 + 25;
   }

   w = mTagImage.width();
   h = mTagImage.height();
   outimg.resize( w,h );
   outimg.setAll(COLOR_RGB(255,255,255));   // white bakcground
   npix = w * h;

   for (i=0; i < npix; i++)
      if ((tag = mTagImage[i]) != -1)  
         outimg.setPix( i, colorMap(tag,0), colorMap(tag,1), colorMap(tag,2) );
   return outimg;
}

/*
Image<UCHAR> connectedComponentSizeFilter(const Image<UCHAR> & img,
                          int connected, int sizeThreshold) {
   ConnectedComponents CC;
   Image<UCHAR> outimg;
   int c,i,j,i1,i2,j1,j2;

	CC.AnalyzeBinary( img, connected );
	outimg = img;
	for (c = 0; c < CC.GetNcomponents(); c++)  {
//		_cprintf("Component %d = %d pixels\n",c,CC.Count(c,1));
		if (CC.Count(c,1) < sizethreshold) {
		  i1 = CC.Bbox( c,0 );  j1 = CC.Bbox( c,1 );
          i2 = CC.Bbox( c,2 );  j2 = CC.Bbox( c,3 );
          for (i = i1; i <= i2; i++)
          for (j = j1; j <= j2; j++)
            if (CC.TagImage(i,j) == c)   outimg(i,j) = 0;
		}
	}
   return outimg;
}
*/
