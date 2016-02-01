#ifndef TYPES_H
#define TYPES_H

typedef unsigned char   byte;

struct vertex
{
    float   x, y;
};

struct color
{
    byte    r, g, b;
};

struct triangle
{
    // vertex indices
    int     i, j, k;

    // color index
    int     c;
};

void    PutPixel(int x, int y, byte r, byte g, byte b);

#endif
