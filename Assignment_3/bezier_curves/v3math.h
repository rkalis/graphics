#ifndef V3MATH_H
#define V3MATH_H

typedef struct
{
    float   x, y, z;
}
vec3;

// Create a new 3-vector of floats
vec3    v3_create(float x, float y, float z);

// Return -a
vec3    v3_negate(vec3 a);

// Return a + b
vec3    v3_add(vec3 a,vec3 b);
// Return a - b
vec3    v3_subtract(vec3 a,vec3 b);

// Return the cross-product of a and b
vec3    v3_crossprod(vec3 a, vec3 b);
// Return the dot-product of a and b
float   v3_dotprod(vec3 a,vec3 b);

// Return a / |a|
vec3    v3_normalize(vec3 a);
// Return s*a
vec3    v3_multiply(vec3 a, float s);

// Return |a|
float   v3_length(vec3 a);

// Return the i-th component of the vector, e.g. for i==0
// this function returns a.x
float   v3_component(vec3 a, int i);

// Set the i-th component of a to v.
// Returns the updated value.
vec3    v3_set_component(vec3 a, int i, float v);

#endif
