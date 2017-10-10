#ifndef COLOR_H_
#define COLOR_H_

#define BOUND(a)(a > 255) ? 255 : ((a > 0) ? a : 0)

typedef struct{
  float red, green, blue;
}Color;

#endif
