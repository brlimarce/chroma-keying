/**********************************************
 * * Chroma Keying
 * 
 * A revision of the sample program provided by
 * Sir Val for his choir singing video with the
 * hue erasing code, hue_erase.cpp, incorporated.
 **********************************************
*/
#define IMAGE_RANGE_CHECK

#include "image.h"
#include "jpegio.h"
#include "color.h"
#include "tools.h"
#include <stdlib.h>
#include <png.h>
#include <dirent.h>

int count_dir_files(const char *folderPath) {
    DIR *dir;
    struct dirent *entry;
    int count = 0;

    if ((dir = opendir(folderPath)) != NULL) {
        while ((entry = readdir(dir)) != NULL) {
            if (entry->d_type == DT_REG) { // Check if it's a regular file
                count++;
            }
        }
        closedir(dir);
    } else {
        perror("Error opening directory");
        exit(EXIT_FAILURE);
    }

    return count;
}

void write_png(const char *filename, int width, int height, png_bytep *rowptr) {
  FILE *fp = fopen(filename, "wb"); // Output file

  if (fp == NULL) {
    fprintf(stderr, "Error Reading file");
    return;
  }

  // Declare the PNG writer structure
  png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL); 

  if (!png_ptr) {
    fprintf(stderr, "Error creating PNG write structure\n");
    fclose(fp);
    return;
  }

  // Create information about the PNG file
  png_infop info_ptr = png_create_info_struct(png_ptr); 
  if (!info_ptr) {
    fprintf(stderr, "Error creating PNG info structure\n");
    png_destroy_write_struct(&png_ptr, NULL);
    fclose(fp);
    return;
  }

  // Settings of PNG writer, e.g. compression type, channels used, etc.
  png_set_IHDR(png_ptr, info_ptr, width, height,
                8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE,
                PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

  png_bytep row_pointers[height]; // PNG rows are copied here

  for (int y = 0; y < height; ++y) {
    row_pointers[y] = rowptr[y];
  }

  // Append PNG rows to PNG structure so it could be written
  // PNG structure is initially empty
  png_set_rows(png_ptr, info_ptr, row_pointers);

  png_init_io(png_ptr, fp); // Streamline
  png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL); // Actual writing

  png_destroy_write_struct(&png_ptr, &info_ptr); // Destroy writer
  fclose(fp);
}

int main(int argc, char *argv[]) {
  int x, y, height, width, pixColor, max_green, min_green, num_frames;
  double red, green, blue;
  double hue, saturation, intensity, min_saturation, max_saturation, min_intensity, max_intensity;
  RGBImage inputImage;

  char image_seq[150], outputFileFormat[100];
  char inputImageFile[100], outputImageFile[100];
  int hue_index;

  if (argc != 3) {
    printf("Usage: chroma_key <folder_with_image_sequence> <output_folder>\n");
    return 0;
  }

  // Read the folder source.
  strcpy(image_seq, argv[1]);
  printf("Root folder is %s\n", image_seq);

  num_frames = count_dir_files(image_seq);

  // Append the file name string format of image sequence.
  strcat(image_seq, "/%04d.jpeg");
  printf("File format string of image sequence: %s\n", image_seq);

  // Read the folder source.
  strcpy(outputFileFormat, argv[2]);
  printf("Root folder is %s\n", outputFileFormat);

  // Append the file name string format of image sequence.
  strcat(outputFileFormat, "/%04d.png");
  printf("File format string of image sequence: %s\n", outputFileFormat);

  printf("Enter min of green hue to filter: ");
  scanf("%i", &min_green);
  printf("Enter max of green hue to filter: ");
  scanf("%i", &max_green);

  printf("Enter min saturation of green hue to filter: ");
  scanf("%lf", &min_saturation);
  printf("Enter max saturation of green hue to filter: ");
  scanf("%lf", &max_saturation);

  printf("Enter min intensity of green hue to filter: ");
  scanf("%lf", &min_intensity);
  printf("Enter max intensity of green hue to filter: ");
  scanf("%lf", &max_intensity);

  for (int frame = 0 ; frame < num_frames; frame++) {
    sprintf(inputImageFile, image_seq, frame);

    readJpeg(inputImage, inputImageFile);
    height = inputImage.height();
    width = inputImage.width();

    // PNG byte pointer array
    // Used for the four channels RGBA per row
    // RED GREEN BLUE (ALPHA - How transparent a pixel should be with range 0 - 255, 0 being very transparent) 
    png_bytep rows[height]; 

    for (x = 0; x < height; x++) {
      // Allocate memory for the four channels row[x] = RED, row[x+1] = Green, row[x+1] = Blue, row[x+2] = Alpha
      rows[x] = (png_byte *) malloc(4 * width * sizeof(png_byte)); 

      if (rows[x] == NULL) {
        fprintf(stderr, "Memory allocation failed for row %d\n", x);

        // Handle the failure, e.g., free previously allocated memory and exit
        for (int i = 0; i < x; i++) {
            free(rows[i]);
        }

        return 0;
      }

      for (y = 0; y < width; y++) {
        pixColor = inputImage(y, x);
        red = RED(pixColor);
        green = GREEN(pixColor);
        blue = BLUE(pixColor);

        rows[x][4 * y] = red;
        rows[x][4 * y + 1] = green;
        rows[x][4 * y + 2] = blue;

        RGBtoHSI(red, green, blue, hue, saturation, intensity);
        hue_index = (int)hue * 180 / 3.14159;

        // Hue that we want to erase
        if (hue_index >= min_green && hue_index <= max_green &&
            saturation >= min_saturation && saturation <= max_saturation &&
            intensity >= min_intensity && intensity <= max_intensity)  {
          rows[x][4 * y + 3] = 0;
        } else {
          rows[x][4 * y + 3] = 255;
        }
      }
    }
    sprintf(outputImageFile, outputFileFormat, frame);
    write_png(outputImageFile, width, height, rows);

    for (x = 0; x < height; x++) {
      free(rows[x]);
    }
  } // Go to the next image in the folder.
}
