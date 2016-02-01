#ifndef TRIRAST_H
#define TRIRAST_H

#include "types.h"

void    draw_triangle(float x0, float y0, float x1, float y1,
            float x2, float y2, byte r, byte g, byte b);
void    draw_triangle_optimized(float x0, float y0, float x1, float y1,
            float x2, float y2, byte r, byte g, byte b);

#endif

