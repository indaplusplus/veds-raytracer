#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <png.h>
#include "image.h"

//https://www.lemoda.net/c/write-png/
int save_as_png(Image *image, char *output_file_name) {
  FILE *f;
  png_structp png;
  png_bytep row;
  png_infop info;
  int return_code;

  if ((f = fopen(output_file_name, "wb")) == NULL)  {
    return_code = 0;
    goto CLEANUP_PNG;
  }

  png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png) {
    return_code = 0;
    goto CLEANUP_PNG;
  }

  info = png_create_info_struct(png);
  if (!info) {
    return_code = 0;
    goto CLEANUP_PNG;
  }

  if (setjmp(png_jmpbuf(png))) {
    return_code = 0;
    goto CLEANUP_PNG;
  }

  png_init_io(png, f);

  //write header, 8 bit bit color depth (RGB)
  png_set_IHDR(png, info, image->width, image->height,
    8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
    PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

  png_write_info(png, info);

  for (int y = 0; y < image->height; y++) {
    row = (png_bytep) malloc(3 * image->width * sizeof(png_byte));
    for (int x = 0; x < image->width; x ++) {
      row[x*3] = image->pixels[(x + y*image->width)*3 + 0];
      row[x*3+1] = image->pixels[(x + y*image->width)*3 + 1];
      row[x*3+2] = image->pixels[(x + y*image->width)*3 + 2];
    }
    png_write_row(png, row);
  }

  png_write_end(png, NULL);
  fclose(f);
  return_code = 1;

  CLEANUP_PNG:
  png_free_data(png, info, PNG_FREE_ALL, -1);

  if (png != NULL)
    png_destroy_write_struct(&png, (png_infopp)NULL);

  if (row != NULL)
    free(row);

  return return_code;
}

int save_as_ppm(Image *image, char *output_file_name) {
  FILE *f;
  if ((f = fopen(output_file_name, "wb")) == NULL)  {
    return 0;
  }
  fprintf(f, "P6 %d %d %d\n", image->width, image->height, 255);
  fwrite(image->pixels, 3, image->width * image->height, f);
  fclose(f);
  return 1;
}

int alloc_pixels(Image *image) {
  size_t image_size = sizeof(unsigned char) * image->width * image->height * 3;
  if ((image->pixels = malloc(image_size)) == NULL) {
    printf("Error, couldn't allocate memory to store the image.\n"); //might be worth moving the prinf
    return 0;
  }
  //use memset to zero the array
  memset(image->pixels, 0, image_size);
  return 1;
}

void destroy_pixels(Image *image) {
  if (image == NULL)
  return;

  free(image->pixels);
}
