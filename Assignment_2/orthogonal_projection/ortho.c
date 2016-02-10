/* Computer Graphics, Assignment 5, Orthogonal projection
 *
 * Filename ........ ortho.c
 * Description ..... Contains the re-programmed orthogonal projection matrix
 * Date ............ 01.09.2006
 * Created by ...... Jurgen Sturm 
 *
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
