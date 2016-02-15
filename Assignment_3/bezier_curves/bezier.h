#ifndef BEZIER_H
#define BEZIER_H

#ifdef OS_X
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

// A single control point of a Bezier curve
typedef struct control_point
{
    float x;
    float y;
}
control_point;

void    evaluate_bezier_curve(float *x, float *y, control_point p[], int num_points, float u);
void    draw_bezier_curve(int num_segments, control_point p[], int num_points);
int     intersect_cubic_bezier_curve(float *y, control_point p[], float x);

#endif
