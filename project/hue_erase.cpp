/**********************************************
 * * Hue Eraser
 * Created by CJ Fabregas, revised by RL Domingo.
 * 
 * This program takes in a JPG/JPEG image as
 * input and outputs a PNG image with the specified
 * hue range erased.
 * 
 * * Disclaimer: Transfer this program file
 * * into Image Lab's folder.
 **********************************************
*/

#include "image.h"
#include "jpegio.h"
#include "color.h"
#include <png.h>

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
    int x, y, width, height, pixel;
    double red, green, blue, hue, saturation, intensity, minimumHue, maximumHue;
    char *output = "images/PNG.png";
    char *input = "images/hue_sat.jpg";

    double pi = 3.14159;
    minimumHue = pi / 4;
    maximumHue = pi;

    RGBImage inputImage;
    readJpeg(inputImage, input);
    width = inputImage.width();
    height = inputImage.height();

    // PNG byte pointer array
    // Used for the four channels RGBA per row
    // RED GREEN BLUE (ALPHA - How transparent a pixel should be with range 0 - 255, 0 being very transparent) 
    png_bytep rows[height]; 

    for (x = 0; x < height; x++) {
        // Allocate memory for the four channels row[x] = RED, row[x+1] = Green, row[x+1] = Blue, row[x+2] = Alpha
        rows[x] = (png_byte *)malloc(4 * width * sizeof(png_byte)); 

        for (y = 0; y < width; y++) {
            pixel = inputImage(y, x);
            red = RED(pixel);
            green = GREEN(pixel);
            blue = BLUE(pixel);

            rows[x][4 * y] = red;
            rows[x][4 * y + 1] = green;
            rows[x][4 * y + 2] = blue;

            RGBtoHSI(red, green, blue, hue, saturation, intensity);

            // Hue that we want to erase
            if (hue > minimumHue && hue < maximumHue)  {
                rows[x][4 * y + 3] = 0;
            } else {
                rows[x][4 * y + 3] = 255;
            }
        }
    }

    write_png(output, width, height, rows);

    for (x = 0; x < height; x++) {
        free(rows[x]);
    }

    return 0;
}
