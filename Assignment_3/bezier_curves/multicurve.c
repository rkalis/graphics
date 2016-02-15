/* Computer Graphics, Assignment, Bezier curves
 * Filename ........ multicurve.c
 * Description ..... Bezier curves
 * Date ............ 22.07.2009
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

#ifdef OS_X
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "bezier.h"
#include "plymodel.h"
#include "physics.h"

#ifndef M_PI
#define M_PI 3.1415926535897932384626
#endif

// World-space values, width corresponds to time, height to velocity
#define EDITOR_GRID_WIDTH       20.0
#define EDITOR_GRID_HEIGHT      4.0
#define EDITOR_MARGIN_FRACTION  0.03

// The number of curves per controled value
#define NUM_LINES                   4
#define NUM_BEZIER_CURVES_PER_LINE  5

// All individual Bezier curves have 4 control points, i.e. they're
// cubic Bezier curves. We're going to connect multiple cubic
// curves together, where the last control point of one curve is used
// as the first control point for the following curve. We represent
// the control points of such a string of Bezier curves using a
// single array of control points. E.g the indices of control points
// in an array of multiple connected curves are as follows:
//
//   curve 0: 0, 1, 2, 3
//   curve 1: 3, 4, 5, 6
//   curve 2: 6, 7, 8, 9,
//   etc.
//
// So for curve i the first end point has index 3*i, the first
// handle point has index 3*i+1, etc.
//
// Per control line we have an array of control points.

control_point
control_points[NUM_LINES][3*NUM_BEZIER_CURVES_PER_LINE+1] =
{
    // Rotation around Y axis
    { {0, 0}, {2, 0}, {4, 0}, {6, 0}, {8, 0}, {10, 0}, {12, 0}, {14, 0}, {16, 0}, {20, 0} },
    // Rotation of first arm joint
    { {0, 0}, {2, 0}, {4, 0}, {6, 0}, {8, 0}, {10, 0}, {12, 0}, {14, 0}, {16, 0}, {20, 0} },
    // Rotation of second arm joint
    { {0, 0}, {2, 0}, {4, 0}, {6, 0}, {8, 0}, {10, 0}, {12, 0}, {14, 0}, {16, 0}, {20, 0} },
    // Grabber joints
    { {0, 0}, {2, 0}, {4, 0}, {6, 0}, {8, 0}, {10, 0}, {12, 0}, {14, 0}, {16, 0}, {20, 0} },
};

int     total_screen_width, total_screen_height;
int 	editor_screen_width, editor_screen_height;
int     view_screen_width, view_screen_height;
float   view_aspect;

// Currently select axes, [0,1,2]
int     current_line = 0;
// Currently selected control point, [0, 3*NUM_BEZIER_CURVES_PER_LINE+1[
int		current_control_point = 0;

// Mouse button currently pressed (and which region is active)
int     mouse_mode = -1;        // GLUT_???_BUTTON value when pressed
int     mouse_region = -1;      // 0 = curve editor, 1 = 3d view
// Where it was pressed, screen coordinates
float   mouse_down_sx, mouse_down_sy;
// Where it was pressed, world coordinates
float   mouse_down_wx, mouse_down_wy;

float   cam_distance = 25.0;
float   cam_rot_z = 45.0, cam_azimuth = 20.0;
float   cam_y = 7.0;
float   saved_cam_rot_z, saved_cam_azimuth, saved_cam_distance, saved_cam_y;

// In milliseconds
#define ANIMATION_STEP 1000.0/60

float   animation_time = 0.0;
int     animation_running = 0;

// Current joint speeds
float   current_rotation_speed, current_joint1_speed, current_joint2_speed, current_grabber_speed;

// Loading/saving curves from/to file

const char *curves_file;
int         curves_modified_since_last_save = 0;

static void
save_curves(const char *fname)
{
    FILE *f = fopen(fname, "wt");
    fprintf(f, "%d %d\n", NUM_LINES, NUM_BEZIER_CURVES_PER_LINE);
    for (int a = 0; a < NUM_LINES; a++)
    {
        for (int cp = 0; cp < 3*NUM_BEZIER_CURVES_PER_LINE+1; cp++)
            fprintf(f, "%.6f %.6f\n", control_points[a][cp].x, control_points[a][cp].y);
    }
    fclose(f);
    printf("Curves saved to %s\n", curves_file);

    curves_modified_since_last_save = 0;
    glutSetWindowTitle("Bezier curves 2");
}

static void
attempt_to_load_curves(const char *fname)
{
    curves_file = fname;

    FILE *f = fopen(fname, "rt");
    if (!f)
    {
        printf("Curves file '%s' could not be opened\n", fname);
        return;
    }

    printf("Reading curves from file '%s'\n", fname);

    int n1, n2;

    if (fscanf(f, "%d %d\n", &n1, &n2) != 2)
    {
        printf("Could not read header line\n");
        return;
    }

    if (n1 != NUM_LINES || n2 != NUM_BEZIER_CURVES_PER_LINE)
    {
        printf("File does not match program settings, not using it!\n");
        return;
    }

    float x, y;

    for (int a = 0; a < NUM_LINES; a++)
    {
        for (int cp = 0; cp < 3*NUM_BEZIER_CURVES_PER_LINE+1; cp++)
        {
            if (fscanf(f, "%f %f\n", &x, &y) != 2)
            {
                printf("Warning: failed to read control point %d.%d\n", a, cp);
                return;
            }
            control_points[a][cp].x = x;
            control_points[a][cp].y = y;
        }
    }

    fclose(f);

    printf("Curves loaded from %s\n", fname);

    curves_modified_since_last_save = 0;
    glutSetWindowTitle("Bezier curves 2");
}

// Determine x, y and z values for the current animation time by
// intersecting the line X=animation_time with the animation curves.

static void
update_curve_intersections(void)
{
    int c;

    for (c = 0; c < NUM_BEZIER_CURVES_PER_LINE; c++)
    {
        if (intersect_cubic_bezier_curve(&current_rotation_speed, &(control_points[0][3*c]), animation_time))
            break;
    }

    for (c = 0; c < NUM_BEZIER_CURVES_PER_LINE; c++)
    {
        if (intersect_cubic_bezier_curve(&current_joint1_speed, &(control_points[1][3*c]), animation_time))
            break;
    }

    for (c = 0; c < NUM_BEZIER_CURVES_PER_LINE; c++)
    {
        if (intersect_cubic_bezier_curve(&current_joint2_speed, &(control_points[2][3*c]), animation_time))
            break;
    }

    for (c = 0; c < NUM_BEZIER_CURVES_PER_LINE; c++)
    {
        if (intersect_cubic_bezier_curve(&current_grabber_speed, &(control_points[3][3*c]), animation_time))
            break;
    }
}

static void
set_animation_time(float t)
{
    if (t > EDITOR_GRID_WIDTH)
    {
        animation_time = EDITOR_GRID_WIDTH;
        animation_running = 0;
        return;
    }

    // Wrap around
    while (t > EDITOR_GRID_WIDTH)
        t -= EDITOR_GRID_WIDTH;

    if (t < 0)
        t = 0;
    else if (t > EDITOR_GRID_WIDTH)
        t = EDITOR_GRID_WIDTH;

    animation_time = t;

    glutPostRedisplay();
}

/// cr, cg, gb is the color of the curve
static void
draw_line_with_extras(int line, int is_selected, float cr, float cg, float cb)
{
    int c;

    for (c = 0; c < NUM_BEZIER_CURVES_PER_LINE; c++)
    {
        // Draw the Bezier curve
        glPushAttrib(GL_LINE_BIT);

            if (is_selected)
            {
                glColor3f(cr, cg, cb);
                glLineWidth(2.0);
            }
            else
            {
                glColor3f(0.8*cr, 0.8*cg, 0.8*cb);
                glLineWidth(1.0);
            }

            draw_bezier_curve(32, &(control_points[line][3*c]), 4);

        glPopAttrib();
    }

    if (is_selected)
    {
        for (c = 0; c < NUM_BEZIER_CURVES_PER_LINE; c++)
        {
            // Draw handles
            glColor3f(0.8, 0.8, 0.8);
            glBegin(GL_LINES);
            glVertex2f(control_points[line][3*c].x, control_points[line][3*c].y);
            glVertex2f(control_points[line][3*c+1].x, control_points[line][3*c+1].y);
            glVertex2f(control_points[line][3*c+2].x, control_points[line][3*c+2].y);
            glVertex2f(control_points[line][3*c+3].x, control_points[line][3*c+3].y);
            glEnd();
        }
    }

    if (is_selected)
    {
        // Draw dots for the control points
        glPointSize(3.0);
        glBegin(GL_POINTS);
        for (c = 0; c < 3*NUM_BEZIER_CURVES_PER_LINE+1; c++)
        {
            if (c % 3 == 0)
            {
                // End point
                glColor3f(cr, cg, cb);
            }
            else
            {
                // Handle point
                glColor3f(0.8, 0.8, 0.8);
            }

            glVertex2f(control_points[line][c].x, control_points[line][c].y);
        }
        glEnd();

        // Draw selected control point
        glColor3f(0, 0, 0);
        glPointSize(5.0);
        glBegin(GL_POINTS);
        glVertex2f(control_points[line][current_control_point].x, control_points[line][current_control_point].y);
        glEnd();

    }
}

static void
draw_line(int line, int selected)
{
    static float colors[4][3] = {
        { 1, 0, 0 },
        { 0, 1, 0 },
        { 0, 0, 1 },
        { 0.5, 0, 1 },
    };

    float r = colors[line][0];
    float g = colors[line][1];
    float b = colors[line][2];

    draw_line_with_extras(line, selected, r, g, b);
}

static void
setup_camera(void)
{
    float	cx, cy, cz;
    float	t;
    float 	beta, gamma;

    glLoadIdentity();

    // degrees -> radians
    beta = cam_azimuth / 180.0 * M_PI;
    gamma = cam_rot_z / 180.0 * M_PI;

    cx = cam_distance;
    cy = cz = 0.0;

    // Rotate around Z
    t = cx;
    cx = cx * cos(beta) + cy * sin(beta);
    cy = t * sin(beta) + cy * cos(beta);

    // Rotate around Y
    t = cx;
    cx = cx * cos(gamma) - cz * sin(gamma);
    cz = t * sin(gamma) + cz * cos(gamma);

    gluLookAt(cx, cy+cam_y, cz,  0.0, cam_y, 0.0,  0.0, 1.0, 0.0);
}


static void
draw_handler(void)
{
    float   grid_step;
    float   grid_pos;
    int     i;

    update_curve_intersections();

    //
    // Draw curve editor
    //

    glClearColor(0.5, 0.5, 0.5, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    glViewport(0, 0, editor_screen_width, editor_screen_height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-EDITOR_MARGIN_FRACTION*EDITOR_GRID_WIDTH, (1+EDITOR_MARGIN_FRACTION)*EDITOR_GRID_WIDTH,
        (-0.5-EDITOR_MARGIN_FRACTION)*EDITOR_GRID_HEIGHT, (0.5+EDITOR_MARGIN_FRACTION)*EDITOR_GRID_HEIGHT,
        -1.0f, 1.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glShadeModel(GL_FLAT);

    // Draw grid

    glColor3f(0.4, 0.4, 0.4);

    glBegin(GL_LINES);

    // parallel to y axis
    grid_pos = 0.0;
    grid_step = 1.0;
    while (grid_pos <= EDITOR_GRID_WIDTH)
    {
        glVertex3f(grid_pos, -0.5*EDITOR_GRID_HEIGHT, 0);
        glVertex3f(grid_pos, 0.5*EDITOR_GRID_HEIGHT, 0);
        grid_pos += grid_step;
    }

    // parallel to x axis
    grid_pos = -0.5*EDITOR_GRID_HEIGHT;
    grid_step = 1.0;
    while (grid_pos <= EDITOR_GRID_HEIGHT)
    {
        glVertex3f(0, grid_pos, 0);
        glVertex3f(EDITOR_GRID_WIDTH, grid_pos, 0);
        grid_pos += grid_step;
    }

    // x axis (time)
    glColor3f(0, 0, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(EDITOR_GRID_WIDTH, 0, 0);

    // y axis (position)
    glVertex3f(0, 0.5*EDITOR_GRID_HEIGHT, 0);
    glVertex3f(0, -0.5*EDITOR_GRID_HEIGHT, 0);

    glEnd();

    // Draw curves

    for (int line = 0; line < NUM_LINES; line++)
    {
        if (current_line != line)
            draw_line(line, 0);
    }

    // Make sure the active line is drawn last, so it's on top
    // of the rest, making it slightly easier to see
    draw_line(current_line, 1);

    // Draw animation timeline

    glBegin(GL_LINES);

    glColor3f(1, 1, 0);
    glVertex3f(animation_time, 0.5*EDITOR_GRID_HEIGHT, 0);
    glVertex3f(animation_time, -0.5*EDITOR_GRID_HEIGHT, 0);

    glEnd();

    // Mark curve values for the current time

    glPointSize(5.0);
    glBegin(GL_POINTS);

    glColor3f(1, 0, 0);
    glVertex3f(animation_time, current_rotation_speed, 0);
    glColor3f(0, 1, 0);
    glVertex3f(animation_time, current_joint1_speed, 0);
    glColor3f(0, 0, 1);
    glVertex3f(animation_time, current_joint2_speed, 0);
    glColor3f(0.5, 0, 1);
    glVertex3f(animation_time, current_grabber_speed, 0);

    glEnd();

    //
    // Draw 3D scene
    //

    // Make sure we only clear the right part of the screen
    glScissor(0, editor_screen_height, view_screen_width, view_screen_height);
    glEnable(GL_SCISSOR_TEST);
    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);

    glViewport(0, editor_screen_height, view_screen_width, view_screen_height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f,(GLfloat)view_screen_width/(GLfloat)view_screen_height, 0.1f, 1000.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    setup_camera();

    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);

    // Grid

    grid_step = 1.0;

    glDisable(GL_LIGHTING);

    glColor3f(0.7, 0.7, 0.7);

    glBegin(GL_LINES);

    // parallel to x axis
    for (i = -10; i <= 10; i++)
    {
        if (i != 0)
        {
            glVertex3f(-10, 0, i*grid_step);
            glVertex3f(10, 0, i*grid_step);
        }
    }

    // parallel to z axis
    for (i = -10; i <= 10; i++)
    {
        if (i != 0)
        {
            glVertex3f(i*grid_step, 0, -10);
            glVertex3f(i*grid_step, 0, 10);
        }
    }

    // The negative x axis
    glVertex3f(-10, 0, 0);
    glVertex3f(0, 0, 0);

    // The negative z axis
    glVertex3f(0, 0, -10);
    glVertex3f(0, 0, 0);

    // The positive x axis
    glColor3f(1, 0, 0);
    glVertex3f(10, 0, 0);
    glVertex3f(0, 0, 0);

    // The positive z axis
    glColor3f(0, 0, 1);
    glVertex3f(0, 0, 10);
    glVertex3f(0, 0, 0);

    glEnd();

    // The simulated objects

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    draw_objects();

    // Swap buffers
    glutSwapBuffers();
}

static void
editor_mouse_to_world(float *wx, float *wy, int mx, int my)
{
    *wx = -EDITOR_MARGIN_FRACTION*EDITOR_GRID_WIDTH + 1.0 * mx / editor_screen_width * (1+2*EDITOR_MARGIN_FRACTION)*EDITOR_GRID_WIDTH;
    *wy = ((1.0*(editor_screen_height-(my-view_screen_height))/editor_screen_height) - 0.5) * (1+2*EDITOR_MARGIN_FRACTION)*EDITOR_GRID_HEIGHT;
}

static void
pick_active_control_point(float wx, float wy)
{
    float   dx, dy;
    float   best_diff;
    int     cp, best_cp;

    dx = control_points[current_line][0].x - wx;
    dy = control_points[current_line][0].y - wy;
    best_diff = dx*dx + dy*dy;
    best_cp = 0;

    for (cp = 1; cp < 3*NUM_BEZIER_CURVES_PER_LINE+1; cp++)
    {
        dx = control_points[current_line][cp].x - wx;
        dy = control_points[current_line][cp].y - wy;

        if (dx*dx + dy*dy < best_diff)
        {
            best_cp = cp;
            best_diff = dx*dx + dy*dy;
        }
    }

    current_control_point = best_cp;

    glutPostRedisplay();
}

static void
mouse_handler(int button, int state, int x, int y)
{
    if (state == GLUT_DOWN && mouse_mode == -1)
    {
        // Button pressed
        if (y > view_screen_height)
        {
            // Curve editor

            if (button == GLUT_RIGHT_BUTTON)
            {
                // Pick closest control point and make it active

                // Compute world coordinates corresponding to mouse coords
                mouse_down_sx = x;
                mouse_down_sy = y;

                float wx, wy;
                editor_mouse_to_world(&wx, &wy, x, y);

                mouse_down_wx = wx;
                mouse_down_wy = wy;

                pick_active_control_point(wx, wy);

                mouse_mode = button;
                mouse_region = 0;
            }
            /*
            else if (button == GLUT_LEFT_BUTTON)
            {
                // Set animation time
                float wx, wy;
                editor_mouse_to_world(&wx, &wy, x, y);

                set_animation_time(wx);

                mouse_mode = button;
                mouse_region = 0;
            }
            */
        }
        else
        {
            // 3D view
            if (button == GLUT_LEFT_BUTTON)
            {
                mouse_mode = button;
                mouse_region = 1;

                saved_cam_rot_z = cam_rot_z;
                saved_cam_azimuth = cam_azimuth;

                mouse_down_sx = x;
                mouse_down_sy = y;
            }
            else if (button == GLUT_MIDDLE_BUTTON)
            {
                mouse_mode = button;
                mouse_region = 1;

                saved_cam_y = cam_y;

                mouse_down_sy = y;

            }
            else if (button == GLUT_RIGHT_BUTTON)
            {
                mouse_mode = button;
                mouse_region = 1;

                saved_cam_distance = cam_distance;

                mouse_down_sy = y;
            }
        }
    }
    else if (state == GLUT_UP && button == mouse_mode)
    {
        // Button for current mode was released
        mouse_mode = -1;
        mouse_region = -1;
    }
}

// Set Y value for current CP to zero
static void
zero_current_control_point(void)
{
    if (current_control_point % 3 == 0)
    {
        // When zeroing an endpoint translate the connected handle point(s)
        // along with the endpoint

        float dy = -control_points[current_line][current_control_point].y;

        if (current_control_point > 0)
        {
            control_points[current_line][current_control_point-1].y += dy;
            if (control_points[current_line][current_control_point-1].y > 0.5*EDITOR_GRID_HEIGHT)
                control_points[current_line][current_control_point-1].y = 0.5*EDITOR_GRID_HEIGHT;
            else if (control_points[current_line][current_control_point-1].y < -0.5*EDITOR_GRID_HEIGHT)
                control_points[current_line][current_control_point-1].y = -0.5*EDITOR_GRID_HEIGHT;
        }

        if (current_control_point < 3*NUM_BEZIER_CURVES_PER_LINE)
        {
            control_points[current_line][current_control_point+1].y += dy;
            if (control_points[current_line][current_control_point+1].y > 0.5*EDITOR_GRID_HEIGHT)
                control_points[current_line][current_control_point+1].y = 0.5*EDITOR_GRID_HEIGHT;
            else if (control_points[current_line][current_control_point+1].y < -0.5*EDITOR_GRID_HEIGHT)
                control_points[current_line][current_control_point+1].y = -0.5*EDITOR_GRID_HEIGHT;
        }
    }

    control_points[current_line][current_control_point].y = 0.0;

    curves_modified_since_last_save = 1;
    glutSetWindowTitle("Bezier curves 2 [edited]");
}

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

static void
translate_current_control_point(float dx, float dy)
{
    float   ori_x, ori_y;
    float   min_x, max_x, min_y, max_y;
    float   new_x, new_y;
    float   t;

    ori_x = control_points[current_line][current_control_point].x;
    ori_y = control_points[current_line][current_control_point].y;

    int cp_type = current_control_point % 3;

    if (cp_type == 0)
    {
        // We're moving an endpoint

        // Don't allow it to be moved outside the time range, nor the
        // vertical range

        min_x = 0.0;
        max_x = EDITOR_GRID_WIDTH;

        min_y = -0.5*EDITOR_GRID_HEIGHT;
        max_y = 0.5*EDITOR_GRID_HEIGHT;

        // Don't allow an endpoint to be moved beyond the handle point
        // of its connected endpoints

        if (current_control_point > 0)
            min_x = MAX(min_x, control_points[current_line][current_control_point-2].x);
        if (current_control_point < 3*NUM_BEZIER_CURVES_PER_LINE)
            max_x = MIN(max_x, control_points[current_line][current_control_point+2].x);

        // Don't allow a connected handle point to move beyond a connected
        // endpoint (as the handle point is moved along with this endpoint)

        if (current_control_point > 0)
        {
            t = control_points[current_line][current_control_point-3].x
                 -
                 control_points[current_line][current_control_point-1].x;
            min_x = MAX(min_x, ori_x+t);
        }

        if (current_control_point < 3*NUM_BEZIER_CURVES_PER_LINE)
        {
            t = control_points[current_line][current_control_point+3].x
                 -
                 control_points[current_line][current_control_point+1].x;
            max_x = MIN(max_x, ori_x+t);
        }

        // Compute the allowed movement

        new_x = ori_x + dx;
        new_y = ori_y + dy;

        if (new_x < min_x)
            new_x = min_x;
        else if (new_x > max_x)
            new_x = max_x;

        if (new_y < min_y)
            new_y = min_y;
        else if (new_y > max_y)
            new_y = max_y;

        // Apply it to the endpoint

        control_points[current_line][current_control_point].x = new_x;
        control_points[current_line][current_control_point].y = new_y;

        // If we move an endpoint of a curve we want the corresponding
        // handle point(s) to move along, so the slope of the curve
        // at the endpoint doesn't change (considerably).

        dx = new_x - ori_x;
        dy = new_y - ori_y;

        if (current_control_point > 0)
        {
            control_points[current_line][current_control_point-1].x += dx;
            control_points[current_line][current_control_point-1].y += dy;
        }

        if (current_control_point < 3*NUM_BEZIER_CURVES_PER_LINE)
        {
            control_points[current_line][current_control_point+1].x += dx;
            control_points[current_line][current_control_point+1].y += dy;
        }
    }
    else
    {
        // We're moving a handle point

        // Don't allow it to be moved outside the time range, nor the
        // vertical range

        min_x = 0.0;
        max_x = EDITOR_GRID_WIDTH;

        min_y = -0.5*EDITOR_GRID_HEIGHT;
        max_y = 0.5*EDITOR_GRID_HEIGHT;

        // We don't want to allow a handle point to be moved beyond
        // the endpoint it's connected to, in terms of X position.
        // We also don't allow the handle point to go beyond the opposite
        // endpoint.

        if (cp_type == 1)
        {
            min_x = MAX(min_x, control_points[current_line][current_control_point-1].x);
            max_x = MIN(max_x, control_points[current_line][current_control_point+2].x);
        }
        else
        {
            min_x = MAX(min_x, control_points[current_line][current_control_point-2].x);
            max_x = MIN(max_x, control_points[current_line][current_control_point+1].x);
        }

        // Compute the allowed movement

        new_x = ori_x + dx;
        new_y = ori_y + dy;

        if (new_x < min_x)
            new_x = min_x;
        else if (new_x > max_x)
            new_x = max_x;

        if (new_y < min_y)
            new_y = min_y;
        else if (new_y > max_y)
            new_y = max_y;

        // Apply it to the handle point

        control_points[current_line][current_control_point].x = new_x;
        control_points[current_line][current_control_point].y = new_y;
    }

    curves_modified_since_last_save = 1;
    glutSetWindowTitle("Bezier curves 2 [edited]");
}

#undef MIN
#undef MAX

static void
motion_handler(int x, int y)
{
    if (mouse_mode == -1)
        return;

    if (mouse_region == 0)
    {
        // Curve editor

        /*
        if (mouse_mode == GLUT_LEFT_BUTTON)
        {
            // Compute world coordinates for current mouse position
            float wx, wy;
            editor_mouse_to_world(&wx, &wy, x, y);
            set_animation_time(wx);
        }
        else */
        if (mouse_mode == GLUT_RIGHT_BUTTON)
        {
            // Compute world coordinates for current mouse position
            float wx, wy;
            editor_mouse_to_world(&wx, &wy, x, y);

            // Translate active control point using difference in mouse
            // position since last mouse move
            float dx = wx - mouse_down_wx;
            float dy = wy - mouse_down_wy;

            translate_current_control_point(dx, dy);

            // Update last mouse down position
            mouse_down_wx = wx;
            mouse_down_wy = wy;

            glutPostRedisplay();
        }
    }
    else
    {
        // 3D view
        int dx, dy;

        if (mouse_mode == GLUT_LEFT_BUTTON)
        {
            dx = mouse_down_sx - x;
            dy = mouse_down_sy - y;

            cam_rot_z = saved_cam_rot_z - dx * 0.25;
            cam_azimuth = saved_cam_azimuth - dy * 0.25;

            if (cam_azimuth > 89.99)
                cam_azimuth = 89.99;
            else if (cam_azimuth < -89.99)
                cam_azimuth = -89.99;
        }
        else if (mouse_mode == GLUT_MIDDLE_BUTTON)
        {
            dy = mouse_down_sy - y;

            cam_y = saved_cam_y - dy * 0.1;
            if (cam_y < -4)
                cam_y = -4;
            else if (cam_y > 20.0)
                cam_y = 20.0;
        }
        else if (mouse_mode == GLUT_RIGHT_BUTTON)
        {
            dy = mouse_down_sy - y;

            cam_distance = saved_cam_distance - dy * 0.25;
            if (cam_distance < 1.5)
                cam_distance = 1.5;
            else if (cam_distance > 100.0)
                cam_distance = 100.0;
        }

        glutPostRedisplay();
    }
}

static void
reshape_handler(int width, int height)
{
    total_screen_width = width;
    total_screen_height = height;

    editor_screen_width = width;
    editor_screen_height = (int)(0.3*height);

    view_screen_width = width;
    view_screen_height = height - editor_screen_height;
    view_aspect = 1.0 * view_screen_width / view_screen_height;
}

static void
timer_handler(int data)
{
    (void)data;

    set_animation_time(animation_time + 0.001 * ANIMATION_STEP);

    sim_step(0.001 * ANIMATION_STEP, current_rotation_speed,
        current_joint1_speed, current_joint2_speed, current_grabber_speed);

    if (animation_running)
        glutTimerFunc(ANIMATION_STEP, timer_handler, 0);
}

static void
key_handler(unsigned char key, int x, int y)
{
    // Keep gcc happy
    (void)x;
    (void)y;

    switch (key) {

    case '1':
    case '2':
    case '3':
    case '4':
        // Selection of active curve. Nice character-based math here :)
        current_line = key - '1';
        glutPostRedisplay();
        break;

    case 'a':
        // Toggle animation, stops at the end of the timeline
        animation_running = 1 - animation_running;
        if (animation_running)
            glutTimerFunc(ANIMATION_STEP, timer_handler, 0);
        break;

    case 'A':
        // Animate from the beginning
        destroy_physics();
        setup_physics();
        //reset_physics();
        animation_time = 0;
        if (!animation_running)
        {
            glutTimerFunc(ANIMATION_STEP, timer_handler, 0);
            animation_running = 1;
        }
        break;

    case 'z':
        zero_current_control_point();
        glutPostRedisplay();
        break;

    case 's':
        save_curves(curves_file);
        break;
    case 'r':
        // Reload curves from file
        attempt_to_load_curves(curves_file);
        glutPostRedisplay();
        break;

    case 'q':
        if (!curves_modified_since_last_save)
        {
            destroy_physics();
            exit(0);
        }
        else
            printf("Curves modified since last save, either [s]ave of [r]eload before quitting\n");

    } // switch
}

int
main(int argc, char **argv)
{
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1024, 768);
    glutCreateWindow("Bezier curves 2");

    glutReshapeFunc(reshape_handler);
    glutKeyboardFunc(key_handler);
    glutMouseFunc(mouse_handler);
    glutMotionFunc(motion_handler);
    glutDisplayFunc(draw_handler);

    if (--argc == 1)
        attempt_to_load_curves(argv[1]);
    else
        attempt_to_load_curves("curves.txt");

    read_ply_model("teapot.ply");

    setup_physics();

    glutMainLoop();

    return 0;
}
