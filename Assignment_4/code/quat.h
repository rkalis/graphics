#ifndef QUAT_H
#define QUAT_H

#include "v3math.h"

// A quaternion, used to represent rotations around arbitrary angles
typedef struct
{
    float x, y, z, w;
}
quat;

// Create a quaternion that represents a rotation of
// angle degrees around the given axis.
quat    quat_create_rotation(vec3 axis, float angle);

// Create a quaternion representation of point p
quat    quat_create_point(vec3 p);

// Calculate quaternion magnitude
float   quat_magnitude(quat q);

// Normalize a quaternion
quat    quat_normalize(quat q);

// Calculate the conjugate (i.e. (-v, w) for quaternion (v, w))
quat    quat_conjugate(quat q);

// Calculate the inverse of a quaterion
quat    quat_inverse(quat q);

// Transform
vec3    quat_transform_vector(quat q, vec3 v);

quat    quat_multiply(quat q, quat r);

#endif
