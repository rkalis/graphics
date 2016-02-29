#include <math.h>
#include "quat.h"
#include "constants.h"

// Create a quaternion that represents a rotation of
// angle degrees around the given axis.
quat
quat_create_rotation(vec3 axis, float angle)
{
    float   sin_a;
    quat    res;

    axis = v3_normalize(axis);

    sin_a = sin(0.5 * (angle/180.0*M_PI));

    res.x = axis.x * sin_a;
    res.y = axis.y * sin_a;
    res.z = axis.z * sin_a;
    res.w = cos(0.5 * (angle/180.0*M_PI));

    return quat_normalize(res);
}

// Create a quaternion representation of point p
quat
quat_create_point(vec3 p)
{
    quat    res;

    res.x = p.x;
    res.y = p.y;
    res.z = p.z;
    res.w = 0.0;

    return res;
}


// Calculate quaternion magnitude
float
quat_magnitude(quat q)
{
    return sqrt(q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w);
}

// Normalize a quaternion
quat
quat_normalize(quat q)
{
    quat    res = q;
    float   inv_mag = 1.0 / quat_magnitude(q);

    res.x *= inv_mag;
    res.y *= inv_mag;
    res.z *= inv_mag;
    res.w *= inv_mag;

    return res;
}


quat
quat_conjugate(quat q)
{
    quat    res = q;

    res.x = -q.x;
    res.y = -q.y;
    res.z = -q.z;

    return res;
}

quat
quat_inverse(quat q)
{
    float   invlen = 1.0 / quat_magnitude(q);
    quat    res = quat_conjugate(q);

    invlen = invlen * invlen;

    res.x *= invlen;
    res.y *= invlen;
    res.z *= invlen;
    res.w *= invlen;

    return res;
}


quat
quat_multiply(quat q, quat r)
{
    quat    res;

	res.w = q.w*r.w - q.x*r.x - q.y*r.y - q.z*r.z;
	res.x = q.w*r.x + q.x*r.w + q.y*r.z - q.z*r.y;
	res.y = q.w*r.y + q.y*r.w + q.z*r.x - q.x*r.z;
	res.z = q.w*r.z + q.z*r.w + q.x*r.y - q.y*r.x;

    return res;
}


