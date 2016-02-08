/* Computer Graphics, Assignment, Translations, Rotations and Scaling
 *
 * Filename ........ transformations.c
 * Description ..... Contains the re-programmed translation, rotation and scaling functions
 *
 * STUDENT 1:
 * Student name .... Robert Jan Schlimbach
 * Student email ... robertjan.schlimbach@student.uva.nl
 * Student ID ...... 10802126
 *
 * STUDENT 2:
 * Student name .... Rosco Kalis
 * Student email ... rosco.kalis@student.uva.nl
 * Student ID ...... 10771603
 *
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

/* Transformation matrix for scaling over x, y and z */
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

/* Transformation matrix for translating over x, y and z */
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

void myRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
    GLfloat u[3], v[3], w[3], t[3];

    /* * * * * * * * * * * * * * * * *
     * Creating an orthogonal basis: *
     * * * * * * * * * * * * * * * * */

    /* Setting and normalising w */
    GLfloat mag_w = sqrt(x * x + y * y + z * z);

    w[0] = x / mag_w;
    w[1] = y / mag_w;
    w[2] = z / mag_w;

    /* Setting t */
    int mindex = find_minimum(w, 3);
    t[0] = w[0];
    t[1] = w[1];
    t[2] = w[2];
    t[mindex] = 1;

    /* Computing and normalising u */
    u[0] = t[1] * w[2] - t[2] * w[1];
    u[1] = t[2] * w[0] - t[0] * w[2];
    u[2] = t[0] * w[1] - t[1] * w[0];

    GLfloat mag_u = sqrt(u[0] * u[0] + u[1] * u[1] + u[2] * u[2]);

    u[0] = u[0] / mag_u;
    u[1] = u[1] / mag_u;
    u[2] = u[2] / mag_u;

    /* Computing v */
    v[0] = w[1] * u[2] - w[2] * u[1];
    v[1] = w[2] * u[0] - w[0] * u[2];
    v[2] = w[0] * u[1] - w[1] * u[0];

    /* * * * * * * * * * * * * * * * *
     * Setting the correct matrices: *
     * * * * * * * * * * * * * * * * */

    /* Converting the angle to radians and computing the sine and cosine */
    angle = -1 * angle * M_PI / 180;
    float cosa = cos(angle);
    float sina = sin(angle);

    GLfloat A[16] =
    {
        u[0], u[1], u[2], 0.0,
        v[0], v[1], v[1], 0.0,
        w[0], w[1], w[2], 0.0,
         0.0,  0.0,  0.0, 1.0
    };

    GLfloat B[16] =
    {
        cosa, -1 * sina, 0.0, 0.0,
        sina,      cosa, 0.0, 0.0,
         0.0,       0.0, 1.0, 0.0,
         0.0,       0.0, 0.0, 1.0
    };

    GLfloat C[16] =
    {
        u[0], v[0], w[0], 0.0,
        u[1], v[1], w[1], 0.0,
        u[2], v[2], w[2], 0.0,
         0.0,  0.0,  0.0, 1.0
    };

    glMultMatrixf(A);
    glMultMatrixf(B);
    glMultMatrixf(C);
}

