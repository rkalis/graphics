// Note: this file is only meant to be #included by main.c

#include "types.h"

struct color colors[] =
{
    { 255, 0, 0 },
    { 0, 255, 0 },
    { 0, 0, 255 },
    { 255, 255, 0 },
    { 0, 255, 255 },
    { 255, 0, 255 },
};

struct vertex  vertices[] =
{
    { 0, 0 },
    { 10, 0 },
    { 0, 10 },
    { 10, 10 },
    { 30, 10 },
    { 10, 30 },
    { 40, 50 },
    { 23, 63 },

    { 60, 30 },
    { 110, 20 },
    { 80, 50 },
    { 100, 55 },
};

struct triangle triangles[] =
{
    // i, j, k,  c
    { 0, 1, 2,  0 },
    { 1, 2, 3,  1 },
    { 3, 4, 5,  2 },

    { 4, 5, 6,  0 },
    { 5, 7, 6,  1 },

    { 8, 9, 10, 2 },
    { 10, 9, 11, 1 },
};
