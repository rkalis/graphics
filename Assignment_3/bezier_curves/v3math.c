#include <math.h>
#include "v3math.h"

vec3
v3_create(float x, float y, float z)
{
    vec3 res;
    res.x = x;
    res.y = y;
    res.z = z;
    return res;
}

vec3
v3_negate(vec3 a)
{
    vec3 res;
    res.x = - a.x;
    res.y = - a.y;
    res.z = - a.z;
    return res;
}

vec3
v3_crossprod(vec3 a, vec3 b)
{
	vec3 res;
	res.x = a.y*b.z - a.z*b.y;
	res.y = a.z*b.x - a.x*b.z;
	res.z = a.x*b.y - a.y*b.x;
	return res;
}

vec3
v3_subtract(vec3 a,vec3 b)
{
	vec3 res;
	res.x = a.x-b.x;
	res.y = a.y-b.y;
	res.z = a.z-b.z;
	return res;
}

vec3
v3_add(vec3 a,vec3 b)
{
	vec3 res;
	res.x = a.x+b.x;
	res.y = a.y+b.y;
	res.z = a.z+b.z;
	return res;
}

vec3
v3_normalize(vec3 a)
{
	vec3 res;
	double l = sqrt(a.x*a.x + a.y*a.y + a.z*a.z);

	res = a;
	res.x /= l;
	res.y /= l;
	res.z /= l;

	return res;
}

float
v3_dotprod(vec3 a, vec3 b)
{
	return a.x*b.x + a.y*b.y + a.z*b.z;
}

vec3
v3_multiply(vec3 a, float s)
{
	vec3 res;
	res.x = a.x*s;
	res.y = a.y*s;
	res.z = a.z*s;
	return res;
}

float
v3_length(vec3 a)
{
	return sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
}

float
v3_component(vec3 a, int i)
{
    if (i == 0)
        return a.x;
    else if (i == 1)
        return a.y;
    else
        return a.z;
}

// Set the i-th component of a to v
vec3
v3_set_component(vec3 a, int i, float v)
{
    vec3    res = a;

    if (i == 0)
        res.x = v;
    else if (i == 1)
        res.y = v;
    else
        res.z = v;

    return res;
}
