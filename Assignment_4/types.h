#ifndef TYPES_H
#define TYPES_H

#include "v3math.h"

typedef unsigned char   byte;

typedef struct
{
    // indices of vertices
    int     v[3];

	// the triangle's normal
    vec3    n;

    // the triangle's vertex normals
    vec3    vn[3];

	// the material index for this triangle
    int     material;
}
triangle;

typedef struct
{
    vec3    center;
    float   radius;

	int     material;
}
sphere;

typedef struct
{
    // The ray t-value at which intersection occurred, used to find
    // the closest hit
    float   t;

    // intersection point
    vec3    p;
    // surface normal at p
    vec3    n;
    // direction from which the ray responsible for this intersection
    // came. points away from the surface!
    vec3    i;

    // the intersected surface's material index
    int     material;

    // level of the ray that caused this intersection;
    // i.e. level 0 is a camera ray, level 1 is is a ray that has
    // been reflected once, etc
    int     ray_level;
}
intersection_point;

typedef struct
{
    vec3    position;
    float   intensity;      // 0 - 1
}
light;

typedef struct
{
    float   azimuth;
    float   rot_z;
    float   distance;
}
viewpoint;

#endif
