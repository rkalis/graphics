#ifndef MARCHING_TETRAHEDRA_H
#define MARCHING_TETRAHEDRA_H

#include "v3math.h"
#include "volume.h"

typedef struct
{
    vec3    p[3];
    vec3    n[3];
}
triangle;

int generate_cell_triangles(triangle *triangles, cell c, unsigned char isovalue);

#endif
