#include <stdio.h>
#include <stdlib.h>
#include "putpixel.h"

void putpixel(int x, int y, int color)
{
  unsigned int *ptr = (unsigned int*)screen->pixels;
  int lineoffset = y * (screen->pitch / 4);
  ptr[lineoffset + x] = color;
}
