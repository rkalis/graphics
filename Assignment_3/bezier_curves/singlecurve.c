/* Computer Graphics, Assignment 6, Bezier curves
 * Filename ........ singlecurve.c
 * Description ..... Bezier curves
 * Date ............ 22.07.2009
 * Created by ...... Paul Melis
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
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bezier.h"

#define MAX_CONTROL_POINTS	6

control_point control_points[MAX_CONTROL_POINTS] =
{
    {10, 20},
    {50, 80},
    {60, 10},
    {90, 60},
	{80, 80},
	{100, 80},
};

int		num_control_points = 4;

int 	screen_width, screen_height;
float 	screen_aspect;
float	world_width, world_height;

int 	segments = 16;
int		current_control_point = 0;

// Mouse button currently pressed (and where it was pressed, world coords)
int     mouse_mode = -1;
float   mouse_down_x, mouse_down_y;

/* cr, cg, gb is the color of the curve.
   pr, pg, pb is the color of the control points
*/
static void
draw_curve_with_extras(control_point p[], int num_points,
	float cr, float cg, float cb, float pr, float pg, float pb)
{
    int i;

	/* Draw straight lines through control points */
	glColor3f(0.8, 0.8, 0.8);
	glBegin(GL_LINE_STRIP);
	for (i = 0; i < num_points; i++)
		glVertex2f(p[i].x, p[i].y);
	glEnd();

	/* Draw the Bezier curve through the points */
	glColor3f(cr, cg, cb);
	draw_bezier_curve(segments, p, num_points);

	/* Draw dots for the control points */
	glPointSize(3.0);
	glBegin(GL_POINTS);
	for (i = 0; i < num_points; i++)
    {
        if (i == current_control_point)
            glColor3f(1, 1, 0);
        else
            glColor3f(pr, pg, pb);
		glVertex2f(p[i].x, p[i].y);
    }
	glEnd();
}

static void
draw_handler(void)
{
    glClearColor(0.7, 0.7, 0.7, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    draw_curve_with_extras(control_points, num_control_points,  1.0, 0.0, 0.0,  0.0, 0.0, 0.0);

    glutSwapBuffers();
}

static void
key_handler(unsigned char key, int x, int y)
{
    switch (key) {

    case 'q':
        exit(0);

	// Curve length in control points
	case '-':
		if (num_control_points >= 3)
			num_control_points--;
		glutPostRedisplay();
		printf("Using %d control points\n", num_control_points);
		break;
	case '+':
	case '=':
		if (num_control_points < MAX_CONTROL_POINTS)
			num_control_points++;
		printf("Using %d control points\n", num_control_points);
		glutPostRedisplay();
		break;

	// Nr. of segments to draw
    case '[':
		segments /= 2;
        if (segments < 1)
            segments = 1;
        printf("Using %d segments to draw Bezier curve\n", segments);
        glutPostRedisplay();
        break;
    case ']':
		segments *= 2;
        printf("Using %d segments to draw Bezier curve\n", segments);
        glutPostRedisplay();
        break;

    // Print control points
    case 'p':
        for (int i = 0; i < num_control_points; i++)
            printf("[%d] %3f %.3f\n", i, control_points[i].x, control_points[i].y);
        break;

    } // switch
}

static void
mouse2world(float *wx, float *wy, int mx, int my)
{
    *wx = 1.0 * mx / screen_width * world_width;
    *wy = 1.0 * (screen_height-my) / screen_height * world_height;
}

static void
mouse_handler(int button, int state, int x, int y)
{
    if (state == GLUT_DOWN)
    {
        // Button was pressed
        if (mouse_mode != -1)
        {
            // Another mouse mode is active, ignore this press
            return;
        }

        if (button == GLUT_RIGHT_BUTTON)
        {
            // Compute world coordinates corresponding to mouse coords

            float wx, wy;
            mouse2world(&wx, &wy, x, y);

            mouse_down_x = wx;
            mouse_down_y = wy;

            // Pick closest control point and make it active

            float   dx, dy;
            float   best_diff;
            int     best_cp;

            dx = control_points[0].x - wx;
            dy = control_points[0].y - wy;

            best_cp = 0;
            best_diff = dx*dx + dy*dy;

            for (int i = 1; i < num_control_points; i++)
            {
                dx = control_points[i].x - wx;
                dy = control_points[i].y - wy;

                if (dx*dx + dy*dy < best_diff)
                {
                    best_cp = i;
                    best_diff = dx*dx + dy*dy;
                }
            }

            current_control_point = best_cp;
            glutPostRedisplay();

            mouse_mode = GLUT_RIGHT_BUTTON;
        }
    }
    else if (button == mouse_mode)
    {
        // Button for current mode was released
        mouse_mode = -1;
    }
}

static void
motion_handler(int x, int y)
{
    // Compute world coordinates for current mouse position
    float wx, wy;
    mouse2world(&wx, &wy, x, y);

    // Translate active control point using difference in mouse position
    // since pressed
    control_points[current_control_point].x += (wx - mouse_down_x);
    control_points[current_control_point].y += (wy - mouse_down_y);

    // Update last mouse down position
    mouse_down_x = wx;
    mouse_down_y = wy;

    glutPostRedisplay();
}

static void
reshape_handler(int width, int height)
{
    screen_width = width;
    screen_height = height;
    screen_aspect = 1.0 * width / height;

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    if (width > height)
    {
        /* make world height 100, scale world width accordingly */
        world_height = 100.0;
        world_width = screen_aspect * world_height;
    }
    else
    {
        /* make world width 100, scale world height accordingly */
        world_width = 100.0;
        world_height = world_width / screen_aspect;
    }

    glOrtho(0.0f, world_width, 0.0, world_height, -1.0f, 1.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int
main(int argc, char **argv)
{
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Bezier curves 1");

    glutReshapeFunc(reshape_handler);
    glutKeyboardFunc(key_handler);
    glutMouseFunc(mouse_handler);
    glutMotionFunc(motion_handler);
    glutDisplayFunc(draw_handler);
    glutMainLoop();

    return 0;
}
