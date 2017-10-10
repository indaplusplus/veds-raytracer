#ifndef IMAGE_H_
#define IMAGE_H_

typedef struct Image Image;
struct Image{
  int width, height;
  unsigned char* pixels;
};

int save_as_png(Image *image, char *output_file_name);
int save_as_ppm(Image *image, char *output_file_name);
int alloc_pixels(Image *image);
void destroy_pixels(Image *image);

#endif
