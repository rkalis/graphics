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

 
#define sqr(x) ((x)*(x))

/* ANSI C/ISO C89 does not specify this constant (?) */
#ifndef M_PI
#define M_PI           3.14159265358979323846  /* pi */
#endif

void myOrtho(GLdouble left,
             GLdouble right,
             GLdouble bottom,
             GLdouble top,
             GLdouble near,
             GLdouble far) {

	/* Scaling matrix */
	GLdouble MorthoScl[16] =
	{
		2 / (right - left), 0, 0, 0,
		0, 2 / (top - bottom), 0, 0,
		0, 0, 2 / (near - far), 0,
		0, 0, 0, 1
	};

	/* Matrix for translating to the origin */
	GLdouble MorthoTrl[16] =
	{
		1, 0, 0, 0,
		0, 1, 1, 0,
		0, 0, 1, 0,
		-1 * (left + right) / 2, -1 * (bottom + top) / 2, -1 * (near + far) / 2, 1
	};

	glMultMatrixd(MorthoScl);
	glMultMatrixd(MorthoTrl);
}
