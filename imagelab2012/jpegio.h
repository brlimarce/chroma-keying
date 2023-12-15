
#include "image.h"

#ifndef JPEGIO_H
#define JPEGIO_H

// return 1 if JPEG image was properly read/written, otherwise return 0
int readJpeg( RGBImage & img, char *name );

int writeJpeg( RGBImage & img, char *name, int quality );


#endif

