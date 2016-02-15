#ifndef TYPES_H
#define TYPES_H

#include "v3math.h"

typedef struct
{
    // indices of vertices
    int     v[3];

	// the triangle's normal
    vec3    n;

	// the material index for this triangle
    int     material;
}
triangle;

#endif
