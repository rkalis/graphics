#ifndef BBOX_H
#define BBOX_H

#include "types.h"

//
// An axis-aligned bounding box (AABB)
//

typedef struct
{
    // The two corners of the bounding box
    vec3    min;
    vec3    max;
}
boundingbox;

// Create a bounding box (initialized to be empty, i.e. min > max)
boundingbox bbox_create(void);

// Update the given bounding box to include the point p
void        bbox_update(boundingbox* bbox, vec3 p);

// Return the bounding box resulting from the combination of the
// two given bounding boxes
boundingbox bbox_combine(boundingbox bbox1, boundingbox bbox2);

// Intersect a ray with given origin and direction (and delimited by
// t0 and t1), with the given bounding box.
// Returns 1 (and sets t_min and t_max) when the ray intersects the bbox,
// returns 0 otherwise
int         bbox_intersect(float* t_min, float* t_max, boundingbox bbox,
                vec3 origin, vec3 direction, float t0, float t1);

// Return the volume of the bounding box
float       bbox_volume(boundingbox bbox);

#endif
