#ifndef SHADERS_H
#define SHADERS_H

#include "types.h"

// Determine the surface color for the first object intersected by
// the given ray, or return the scene background color when no
// intersection is found
vec3    ray_color(int level, vec3 ray_origin, vec3 ray_direction);

#endif
