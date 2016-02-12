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

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
 
/* ANSI C/ISO C89 does not specify this constant (?) */
#ifndef M_PI
#define M_PI           3.14159265358979323846  /* pi */
#endif


// Calculate the norm of a given vector in 3D.
GLdouble calculate_norm(GLdouble x, GLdouble y, GLdouble z){
    GLdouble norm = sqrt(x * x + y * y + z * z);
    return norm;
}

// Normalize a given vector in 3D.
void normalize(GLdouble *v){
    GLdouble norm = calculate_norm(v[0], v[1], v[2]);

    v[0] = v[0] / norm;
    v[1] = v[1] / norm;
    v[2] = v[2] / norm;
}

// Calculate the crossproduct of 2 given vectors in 3D,
// and put the results in 'return_v'
void calculate_crossproduct(GLdouble* v0, GLdouble* v1, GLdouble* return_v){
    return_v[0] = v0[1] * v1[2] - v1[1] * v0[2];
    return_v[1] = v0[2] * v1[0] - v1[2] * v0[0];
    return_v[2] = v0[0] * v1[1] - v1[0] * v0[1];
}

void myLookAt(GLdouble eyeX, GLdouble eyeY, GLdouble eyeZ,
              GLdouble centerX, GLdouble centerY, GLdouble centerZ,
              GLdouble upX, GLdouble upY, GLdouble upZ) {


    GLdouble n[3], u[3], v[3], up[3], eye[3];

    // Generate the 'up' vector.
    up[0] = upX;
    up[1] = upY;
    up[2] = upZ;

    // Generate the 'eye' vector.
    eye[0] = eyeX;
    eye[1] = eyeY;
    eye[2] = eyeZ;

    // Generate the 'n' vector, the vector pointing from eye to center.
    n[0] = eye[0] - centerX;
    n[1] = eye[1] - centerY;
    n[2] = eye[2] - centerZ;

    normalize(n);

    // Calculate the 'u' vector meaning the perpendicular vector between up and n.
    calculate_crossproduct(up, n, u);
    normalize(u);

    // Calculate te 'v' vector.
    calculate_crossproduct(n, u, v);
    normalize(v);

    // The final matrix
    GLdouble r_t[16] =
    {
        u[0], v[0], n[0], 0,
        u[1], v[1], n[1], 0,
        u[2], v[2], n[2], 0,
        0.0,   0.0,    0.0,   1.0
    };

    glMultMatrixd(r_t);

    // Translate the matrix to account for a non [0,0,0] object.
    glTranslated(-eyeX, -eyeY, -eyeZ);
}
