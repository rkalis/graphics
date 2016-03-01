#include "constants.h"
#include "v3math.h"
#include "bbox.h"
#include "intersection.h"

// Create a bounding box (initialized to be empty, i.e. min > max)
boundingbox
bbox_create(void)
{
    boundingbox bbox;

    bbox.min = v3_create(C_INFINITY, C_INFINITY, C_INFINITY);
    bbox.max = v3_create(-C_INFINITY, -C_INFINITY, -C_INFINITY);

    return bbox;
}

// Update the given bounding box to include the point p
void
bbox_update(boundingbox* bbox, vec3 p)
{
    if (p.x < bbox->min.x) bbox->min.x = p.x;
    if (p.y < bbox->min.y) bbox->min.y = p.y;
    if (p.z < bbox->min.z) bbox->min.z = p.z;

    if (p.x > bbox->max.x) bbox->max.x = p.x;
    if (p.y > bbox->max.y) bbox->max.y = p.y;
    if (p.z > bbox->max.z) bbox->max.z = p.z;
}

// Return the bounding box resulting from the combination of the
// two given bounding boxes
boundingbox
bbox_combine(boundingbox bbox1, boundingbox bbox2)
{
    boundingbox bbox = bbox1;

    if (bbox2.min.x < bbox.min.x) bbox.min.x = bbox2.min.x;
    if (bbox2.min.y < bbox.min.y) bbox.min.y = bbox2.min.y;
    if (bbox2.min.z < bbox.min.z) bbox.min.z = bbox2.min.z;

    if (bbox2.max.x > bbox.max.x) bbox.max.x = bbox2.max.x;
    if (bbox2.max.y > bbox.max.y) bbox.max.y = bbox2.max.y;
    if (bbox2.max.z > bbox.max.z) bbox.max.z = bbox2.max.z;

    return bbox;
}


// Intersect a ray with given origin and direction, with the given
// bounding box.
// Returns 1 (and sets t_min and t_max) when an intersection is found,
// returns 0 otherwise
int
bbox_intersect(float* t_min, float* t_max, boundingbox bbox,
    vec3 origin, vec3 direction, float t0, float t1)
{
    // This code makes handy use of IEEE floating-point behaviour, such
    // as division by zero. See
    // 1) Shirley et.al. Section 10.9.1
    // 2) Williams et.al., "An Efficient and Robust Ray-Box Intersection Algorithm"

    float   tmin, tmax;
    float   tymin, tymax, tzmin, tzmax;
    float   inv;

    num_bboxes_tested++;

    inv = 1.0 / direction.x;
    if (inv >= 0.0)
    {
        tmin = (bbox.min.x - origin.x) * inv;
        tmax = (bbox.max.x - origin.x) * inv;
    }
    else
    {
        tmin = (bbox.max.x - origin.x) * inv;
        tmax = (bbox.min.x - origin.x) * inv;
    }

    inv = 1.0 / direction.y;
    if (inv >= 0.0)
    {
        tymin = (bbox.min.y - origin.y) * inv;
        tymax = (bbox.max.y - origin.y) * inv;
    }
    else
    {
        tymin = (bbox.max.y - origin.y) * inv;
        tymax = (bbox.min.y - origin.y) * inv;
    }

    if (tmin > tymax || tymin > tmax)
        return 0;

    if (tymin > tmin)
        tmin = tymin;
    if (tymax < tmax)
        tmax = tymax;

    inv = 1.0 / direction.z;
    if (inv >= 0.0)
    {
        tzmin = (bbox.min.z - origin.z) * inv;
        tzmax = (bbox.max.z - origin.z) * inv;
    }
    else
    {
        tzmin = (bbox.max.z - origin.z) * inv;
        tzmax = (bbox.min.z - origin.z) * inv;
    }

    if (tmin > tzmax || tzmin > tmax)
        return 0;
    if (tzmin > tmin)
        tmin = tzmin;
    if (tzmax < tmax)
        tmax = tzmax;

    *t_min = tmin;
    *t_max = tmax;

    return (tmin < t1 && tmax > t0);
}

float
bbox_volume(boundingbox bbox)
{
    return (bbox.max.x - bbox.min.x) * (bbox.max.y - bbox.min.y) * (bbox.max.z - bbox.min.z);
}
