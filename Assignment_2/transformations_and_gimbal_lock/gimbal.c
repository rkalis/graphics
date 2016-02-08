/* Computer Graphics, Assignment, Translations, Rotations and Scaling
 *
 * Filename ........ gimbal.c
 * Description ..... Draw teapots that can be interactively rotated with the mouse
 * Created by ...... Paul Melis
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

#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "transformations.h"
#include <math.h>

#define ROTATE_X_AND_Z	0
#define ROTATE_ONLY_X	1
#define ROTATE_ONLY_Z	2

int     window;
float   x_rotation=0.0, z_rotation=0.0;
int     prev_mouse_x, prev_mouse_y;
int     do_mouse_transform=0;
int     rotation_mode=ROTATE_X_AND_Z;

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
    float hfov=90.0, vfov;

    if (Height == 0)
        Height=1;

    glViewport(0, 0, Width, Height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    vfov = hfov / (1.0*Width/Height);

    gluPerspective(vfov, (GLfloat)Width/(GLfloat)Height, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}

void drawRotatedTeapot(float rotx, float roty, float rotz)
{
    glRotatef(rotx, 1.0, 0.0, 0.0);
    glRotatef(roty, 0.0, 1.0, 0.0);
    glRotatef(rotz, 0.0, 0.0, 1.0);

    // teapot

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glutSolidTeapot(1.0);

    // local coordinate axes

    glDisable(GL_LIGHTING);
    glLineWidth(2.0);

    glBegin(GL_LINES);
    // Red: X-axis
    glColor3f(1, 0, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(2, 0, 0);

    // Green: Y-axis
    glColor3f(0, 1, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 2, 0);

    // Blue: Z-axis
    glColor3f(0, 0, 1);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, 2);
    glEnd();
}

void drawTeapots(void)
{
    /* This function is called from DrawGLScene() below */

    glPushMatrix();

    drawRotatedTeapot(x_rotation, 0.0, z_rotation);

    glPopMatrix();
}

void DrawGLScene(void)
{
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glLoadIdentity ();
    gluLookAt (5.0, 6.0, 9.0,  5.0, 0.0, 0.0,   0.0, 1.0, 0.0);

    drawTeapots();

    glutSwapBuffers();
}

void keyPressed(unsigned char key, int x, int y)
{
    switch(key) {
    case 27:
      glutDestroyWindow(window);
      exit(0);
    case 'r':
      x_rotation = 0.0f;
      z_rotation = 0.0f;
      rotation_mode = ROTATE_X_AND_Z;
      printf("X and Z rotation reset to 0 degrees\n");
      printf("Mouse rotates around both X and Z axes\n");
      break;
    case 'x':
      rotation_mode = ROTATE_ONLY_X;
      printf("Mouse rotates only around X axis\n");
      break;
    case 'z':
      rotation_mode = ROTATE_ONLY_Z;
      printf("Mouse rotates only around Z axis\n");
      break;
    }

    DrawGLScene();
}

void mouseClick(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON)
    {
        if (state == GLUT_DOWN)
        {
            prev_mouse_x = x;
            prev_mouse_y = y;
            do_mouse_transform = 1;
        }
        else
        {
            // mouse button released
            do_mouse_transform = 0;
        }
    }
}

void mouseMove(int x, int y)
{
    int dx, dy;

    if (do_mouse_transform)
    {
        dx = x - prev_mouse_x;
        dy = y - prev_mouse_y;

        if (rotation_mode != ROTATE_ONLY_Z)
            x_rotation += dy * 0.25f;
        if (rotation_mode != ROTATE_ONLY_X)
            z_rotation -= dx * 0.25f;

        prev_mouse_x = x;
        prev_mouse_y = y;
    }
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(0, 0);
    window = glutCreateWindow("Gimbal lock");

    glutDisplayFunc(&DrawGLScene);
    glutIdleFunc(&DrawGLScene);
    glutReshapeFunc(&ReSizeGLScene);
    glutKeyboardFunc(&keyPressed);
    glutMouseFunc(&mouseClick);
    glutMotionFunc(&mouseMove);

    InitGL();

    glutMainLoop();

    return 1;
}




