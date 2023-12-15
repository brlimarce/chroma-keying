/**********************************************
 * * Chroma Keying Example
 * A sample program provided by Sir Val for 
 * his choir singing video.
 * 
 * A lot of the values are hardcoded, like the
 * number of frames, etc.
 * 
 * * Disclaimer: Transfer this program file
 * * into Image Lab's folder.
 **********************************************
*/
#define IMAGE_RANGE_CHECK

#include "image.h"
#include "jpegio.h"
#include "color.h"
#include "tools.h"
#include <stdlib.h>

int main (int argc, char *argv[]) {
  int x, y, height,width, pixColor, max_green, min_green;
  double red, green, blue;
  double hue, saturation, intensity, satNew, intNew;
  RGBImage inputImage, outputImage, backgroundImage, croppedImage;

  char image_seq[150], outputFileFormat[100];
  char inputImageFile[100], outputImageFile[100];
  int hue_index;

  if (argc != 4) {
    printf("Usage: chroma_key <background image> <folder_with_image_sequence> <output_folder>\n");
    return 0;
  }

  // Read image background.
  // TODO: Remove lines 37 - 39.
  readJpeg(backgroundImage, argv[1]);
  height = backgroundImage.height();
  width = backgroundImage.width();

  // Read the folder source.
  strcpy(image_seq, argv[2]);
  printf("Root folder is %s\n", image_seq);

  // Append the file name string format of image sequence.
  strcat(image_seq, "\\%04d.jpg");
  printf("File format string of image sequence: %s\n", image_seq);

  // Read the folder source.
  strcpy(outputFileFormat, argv[3]);
  printf("Root folder is %s\n", outputFileFormat);

  // Append the file name string format of image sequence.
  strcat(outputFileFormat, "\\%04d.jpg");
  printf("File format string of image sequence: %s\n", outputFileFormat);

  outputImage.resize(width, height); // Set the dimensions.

  printf("Enter min of green hue to filter: ");
  scanf("%i", &min_green);
  printf("Enter max of green hue to filter: ");
  scanf("%i", &max_green);

  for (int frame = 0 ; frame < 2591; frame++) {
    sprintf(inputImageFile, image_seq, frame);
    readJpeg(inputImage, inputImageFile);

    // * Additional Code
    // Crop the image.
    // 0: start point for width
    // width - 1: end point for width
    //
    // 1: start point for height
    // height - 1: end point for height
    cropImageRGB(croppedImage, inputImage, 0, 1, width - 1, height - 1);

    for (x = 0; x < width; x++) {
      for (y = 0; y < height; y++) {
      // Read each pixel in the input image.
      pixColor = inputImage(x, y);

      // Get the red, green, and blue components.
      red = RED(pixColor);
      green = GREEN(pixColor);
      blue = BLUE(pixColor);

      // Convert from RGB to HSI .
      RGBtoHSI(red, green, blue, hue, saturation, intensity);

      // Get index in an array of degrees.
      hue_index = (int)hue * 180 / 3.14159;

      // If it is 'GREEN', set to background image.
      // TODO: Make the pixel transparent (instead of the background image).
      if (hue_index >= min_green && hue_index <= max_green && intensity > 0.3)
        outputImage(x, y) = backgroundImage(x, y);
      else
        outputImage(x, y) = pixColor;
      }
    }

    sprintf(outputImageFile, outputFileFormat, frame);
    writeJpeg(outputImage, outputImageFile, 85);
  } // Go to the next image in the folder.
}
