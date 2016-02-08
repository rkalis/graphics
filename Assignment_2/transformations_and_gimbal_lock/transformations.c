/* Computer Graphics, Assignment, Translations, Rotations and Scaling
 *
 * Filename ........ transformations.c
 * Description ..... Contains the re-programmed translation, rotation and scaling functions
 * Student name ....
 * Student email ...
 * Collegekaart ....
 * Date ............
 * Comments ........
 *
 *
 * (always fill in these fields before submitting!!)
 */

#ifdef OS_X
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "transformations.h"
#include <assert.h>

/* ANSI C/ISO C89 does not specify this constant (?) */
#ifndef M_PI
#define M_PI           3.14159265358979323846  /* pi */
#endif

/* Returns the index of the minimum element in an array.
 * Found at http://goo.gl/kDQhRP
 */
int find_minimum(float a[], int n) {
    int c, min, index;

    min = a[0];
    index = 0;

    for (c = 1; c < n; c++) {
    if (a[c] < min) {
        index = c;
        min = a[c];
    }
}
 
  return index;
}

void myScalef(GLfloat x, GLfloat y, GLfloat z)
{
    GLfloat M[16] =
    {
          x, 0.0, 0.0, 0.0,
        0.0,   y, 0.0, 0.0,
        0.0, 0.0,   z, 0.0,
        0.0, 0.0, 0.0, 1.0
    };

    glMultMatrixf(M);
}


void myTranslatef(GLfloat x, GLfloat y, GLfloat z)
{
    GLfloat M[16] =
    {
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
          x,   y,   z, 1.0
    };

    glMultMatrixf(M);
}

float dot_product3D(GLfloat vector1[], GLfloat vector2[]) {
    return vector1[0] * vector2[0] + vector1[1] * vector2[1] + vector1[2] * vector2[2];
}

void myRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
    GLfloat u[3], v[3], w[3], t[3];

    //
    // 1. Create the orthonormal basis
    //
    GLfloat mag_w = sqrt(x * x + y * y + z * z);
    w[0] = x / mag_w;
    w[1] = y / mag_w;
    w[2] = z / mag_w; 
    assert(sqrt(w[0] * w[0] + w[1] * w[1] + w[2] * w[2]) == 1);
    int mindex = find_minimum(w, 3);
    t[0] = w[0];
    t[1] = w[1];
    t[2] = w[2];
    t[mindex] = 1;

    u[0] = t[1] * w[2] - t[2] * w[1];
    u[1] = t[2] * w[0] - t[0] * w[2];
    u[2] = t[0] * w[1] - t[1] * w[0];

    GLfloat mag_u = sqrt(u[0] * u[0] + u[1] * u[1] + u[2] * u[2]);
    u[0] = u[0] / mag_u;
    u[1] = u[1] / mag_u;
    u[2] = u[2] / mag_u;
    assert(sqrt(u[0] * u[0] + u[1] * u[1] + u[2] * u[2]) == 1);

    v[0] = w[1] * u[2] - w[2] * u[1];
    v[1] = w[2] * u[0] - w[0] * u[2];
    v[2] = w[0] * u[1] - w[1] * u[0];

    printf("u: %f, %f, %f\n", u[0], u[1], u[2]);
    printf("v: %f, %f, %f\n", v[0], v[1], v[2]);
    printf("w: %f, %f, %f\n", w[0], w[1], w[2]);

    // Store the incoming rotation axis in w and normalize w

    // Compute the value of t, based on w

    // Compute u = t x w

    // Normalize u

    // Compute v = w x u

    // At this point u, v and w should form an orthonormal basis.
    // If your routine does not seem to work correctly it might be
    // a good idea to the check the vector values.

    //
    // 2. Set up the three matrices making up the rotation
    //

    // Specify matrix A

    GLfloat C[16] =
    {
        u[0], v[0], w[0], 0.0,
        u[1], v[1], w[1], 0.0,
        u[2], v[2], w[2], 0.0,
         0.0,  0.0,  0.0, 1.0
    };

    // Convert 'angle' to radians
    angle = -1 * angle * M_PI / 180;
    // Specify matrix B

    GLfloat B[16] =
    {
        cos(angle), -1 * sin(angle), 0.0, 0.0,
        sin(angle),      cos(angle), 0.0, 0.0,
               0.0,             0.0, 1.0, 0.0,
               0.0,             0.0, 0.0, 1.0
    };

    // Specify matrix C

    GLfloat A[16] =
    {
        u[0], u[1], u[2], 0.0,
        v[0], v[1], v[1], 0.0,
        w[0], w[1], w[2], 0.0,
         0.0,  0.0,  0.0, 1.0
    };

    //
    // 3. Apply the matrices to get the combined rotation
    //

    glMultMatrixf(A);
    glMultMatrixf(B);
    glMultMatrixf(C);
}

