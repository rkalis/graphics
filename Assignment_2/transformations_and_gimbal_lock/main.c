/* Computer Graphics, Assignment 3, Translations, Rotations and Scaling
 *
 * Filename ........ main.c
 * Description ..... Creates OpenGL window and draws the scene.
 * Created by ...... Jurgen Sturm
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "transformations.h"
#include <math.h>

int window;
int useMyTransformations=0;

int frame=0;
int doRotate=1;
int doTranslate=1;
int doScale=1;
double speed=0.005;

void InitGL(void)
{
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClearDepth(1.0);
  glDepthFunc(GL_LESS);
  glEnable(GL_DEPTH_TEST);
  glShadeModel(GL_SMOOTH);
}

void ReSizeGLScene(int Width, int Height)
{
  if (Height==0)
    Height=1;

  glViewport(0, 0, Width, Height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0f, (GLfloat) Width / (GLfloat) Height, 0.1f, 100.0f);
  glMatrixMode(GL_MODELVIEW);
}

void drawCube(void) {
  glBegin(GL_QUADS);				/* start drawing the cube. */

  /* top of cube */
  glColor3f(0.0f,1.0f,0.0f);
  glVertex3f( 1.0f, 1.0f,-1.0f);
  glVertex3f(-1.0f, 1.0f,-1.0f);
  glVertex3f(-1.0f, 1.0f, 1.0f);
  glVertex3f( 1.0f, 1.0f, 1.0f);

  /* bottom of cube */
  glColor3f(1.0f,0.5f,0.0f);
  glVertex3f( 1.0f,-1.0f, 1.0f);
  glVertex3f(-1.0f,-1.0f, 1.0f);
  glVertex3f(-1.0f,-1.0f,-1.0f);
  glVertex3f( 1.0f,-1.0f,-1.0f);

  /* front of cube */
  glColor3f(1.0f,0.0f,0.0f);
  glVertex3f( 1.0f, 1.0f, 1.0f);
  glVertex3f(-1.0f, 1.0f, 1.0f);
  glVertex3f(-1.0f,-1.0f, 1.0f);
  glVertex3f( 1.0f,-1.0f, 1.0f);

  /* back of cube. */
  glColor3f(1.0f,1.0f,0.0f);
  glVertex3f( 1.0f,-1.0f,-1.0f);
  glVertex3f(-1.0f,-1.0f,-1.0f);
  glVertex3f(-1.0f, 1.0f,-1.0f);
  glVertex3f( 1.0f, 1.0f,-1.0f);

  /* left of cube */
  glColor3f(0.0f,0.0f,1.0f);
  glVertex3f(-1.0f, 1.0f, 1.0f);
  glVertex3f(-1.0f, 1.0f,-1.0f);
  glVertex3f(-1.0f,-1.0f,-1.0f);
  glVertex3f(-1.0f,-1.0f, 1.0f);

  /* Right of cube */
  glColor3f(1.0f,0.0f,1.0f);
  glVertex3f( 1.0f, 1.0f,-1.0f);
  glVertex3f( 1.0f, 1.0f, 1.0f);
  glVertex3f( 1.0f,-1.0f, 1.0f);
  glVertex3f( 1.0f,-1.0f,-1.0f);
  glEnd();

}

void drawTeapot(void)
{
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glutSolidTeapot(2.0);
}

void DrawGLScene(void)
{
  double stretch,height,position,rotation;

  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

  glLoadIdentity();
  gluLookAt(5.0, 5.0, 20.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

  position=cos((double)frame*speed)*2;
  height=fabs(sin((double)frame*speed)*10)+1;
  rotation=cos((double)frame*speed)*90;
  stretch=1+0.3*sin((double)frame*speed/2);

  if(useMyTransformations) {
    if(doTranslate) myTranslatef(position,height-5,0.0);
    if(doRotate) myRotatef(rotation,0.2,0.6,0.77);
    if(doScale) myScalef(1/sqrt(stretch),stretch,1/sqrt(stretch));
  } else {
    if(doTranslate) glTranslatef(position,height-5,0.0);
    if(doRotate) glRotatef(rotation,0.2,0.6,0.77);
    if(doScale) glScalef(1/sqrt(stretch),stretch,1/sqrt(stretch));
  }

  //drawCube();
  drawTeapot();

  frame++;

  glutSwapBuffers();
}

void keyPressed(unsigned char key, int x, int y)
{
  switch(key) {
    case 27:
    case 'q':
      glutDestroyWindow(window);
      exit(0);
    case 'r':
      doRotate = !doRotate;
      break;
    case 't':
      doTranslate = !doTranslate;
      break;
    case 's':
      doScale = !doScale;
      break;
    case '+':
      speed *= 1.1;
      frame = 0;
      break;
    case '-':
      speed /= 1.1;
      frame = 0;
      break;
    default:
      useMyTransformations = !useMyTransformations;
      if (useMyTransformations)
        glutSetWindowTitle("Your transformations");
      else
        glutSetWindowTitle("OpenGL transformations");
  }

  glutPostRedisplay();
}

int main(int argc, char **argv)
{
  glutInit(&argc, argv);

  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);
  glutInitWindowSize(640, 480);
  glutInitWindowPosition(0, 0);
  window = glutCreateWindow("OpenGL transformations");

  glutDisplayFunc(DrawGLScene);
  glutIdleFunc(DrawGLScene);
  glutReshapeFunc(ReSizeGLScene);
  glutKeyboardFunc(keyPressed);

  InitGL();

  glutMainLoop();

  return 0;
}
