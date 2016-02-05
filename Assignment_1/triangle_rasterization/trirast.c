/* 
 * Computer Graphics assignment, Triangle Rasterization
 * Filename ........ trirast.c
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

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "types.h"

/* Defines the offscreen point for drawing edges */ 
int OFFSCREEN[] = {-1, -1};

/* This returns the maximum value of three passed argument by calling the fmax
 * function twice.
 */
float max_3(float a, float b, float c) {
    float max_a_b = fmax(a, b);
    return fmax(max_a_b, c);
}

/* This returns the minimum value of three passed argument by calling the fmin
 * function twice.
 */
float min_3(float a, float b, float c) {
    float min_a_b = fmin(a, b);
    return fmin(min_a_b, c);
}

/* The function that checks whether a point (x, y) is on a line
 * defined by the implicit equation:
 *    (ya - yb) * x + (xb - xa) * y + xa * yb - xb * ya = 0
 */
float f(float xa, float xb, float ya, float yb, int x, int y) {
    return (ya - yb) * x + (xb - xa) * y + xa * yb - xb * ya;
}

/* Returns the increment of one step in the line defined by function f */
int f_incr(float xa, float xb, float ya, float yb, float sign) {
    return (int) ((ya - yb + xb - xa) * sign);
}

/* This draws a triangle with points (x0, y0), (x1, y1), (x2, y2) in the colour
 * defined by (r, g, b). It uses the computation of barycentric coordinates
 * to check whether each point is inside the triangle. It only checks points
 * that are inside the bounding box of the triangle.
 */
void draw_triangle(float x0, float y0, float x1, float y1, float x2, float y2,
    byte r, byte g, byte b) {

    /* Finds the bounding box of the triangle */
    int xmin = (int) floor(min_3(x0, x1, x2));
    int xmax = (int) ceil(max_3(x0, x1, x2));
    int ymin = (int) floor(min_3(y0, y1, y2));
    int ymax = (int) ceil(max_3(y0, y1, y2));

    float alpha, beta, gamma;

    /* Equations for the alpha, beta and gamma points in respect to the lines
     * of the triangle */
    float alpha_function = f(x1, x2, y1, y2, x0, y0);
    float beta_function = f(x2, x0, y2, y0, x1, y1);
    float gamma_function = f(x0, x1, y0, y1, x2, y2);

    /* Equations for the offscreen points in respect to the 
     * lines of the triangle */
    float alpha_function_offscreen = f(x1, x2, y1, y2, OFFSCREEN[0], OFFSCREEN[1]);
    float beta_function_offscreen  = f(x2, x0, y2, y0, OFFSCREEN[0], OFFSCREEN[1]);
    float gamma_function_offscreen = f(x0, x1, y0, y1, OFFSCREEN[0], OFFSCREEN[1]);

    for(int y = ymin; y <= ymax; y++) {
        for(int x = xmin; x <= xmax; x++) {
            /* Computes the new alpha, beta and gamma values and divides by
             * The function value of the corresponding lines to get a value
             * between 0 and 1 when it is inside the triangle, below zero
             * when not.
             */
            alpha = f(x1, x2, y1, y2, x, y) / alpha_function;
            beta = f(x2, x0, y2, y0, x, y) / beta_function;
            gamma = f(x0, x1, y0, y1, x, y) / gamma_function;

            if(alpha >= 0 && beta >= 0 && gamma >= 0) {
                /* Only draw if it's inside the triangle, or on the triangle edge
                 * if the triangle is on the side of the arbitrarily chosen
                 * offscreen point. This prevents shared edges being drawn twice
                 */
                if((alpha > 0 || alpha_function * alpha_function_offscreen > 0)
                && (beta > 0 || beta_function * beta_function_offscreen > 0)
                && (gamma > 0 || gamma_function * gamma_function_offscreen > 0)) {
                    PutPixel(x, y, r, g, b);
                }
            }
        }
    }
}

/* An optimised version of the draw_triangle function. It uses an incremental
 * approach to computing the barycentric coordinates, only computing a start
 * value and then incrementing that value every step along the way, using
 * f_incr. It also gets rid of all floating point operations, favouring
 * integers instead. This means that the alpha, beta and gamma values are not
 * between 0 and 1 when a point is within the triangle, but instead it is just
 * greater than or equal to 0, so it can still be tested equally well.
 * If you were to use it for interpolation (eg. of colours) however, you would
 * run into problems.
 */
void draw_triangle_optimized(float x0, float y0, float x1, float y1, float x2, float y2,
    byte r, byte g, byte b) {

    /* Finds the bounding box of the triangle */
    int xmin = (int) floor(min_3(x0, x1, x2));
    int xmax = (int) ceil(max_3(x0, x1, x2));
    int ymin = (int) floor(min_3(y0, y1, y2));
    int ymax = (int) ceil(max_3(y0, y1, y2));

    /* Finds the sign of all points in respect to the lines */
    int alpha_sign = 1;
    int beta_sign  = 1;
    int gamma_sign = 1;
    if(signbit(f(x1, x2, y1, y2, x0, y0))) 
        alpha_sign = -1;
    if(signbit(f(x2, x0, y2, y0, x1, y1)))
        beta_sign = -1;
    if(signbit(f(x0, x1, y0, y1, x2, y2)))
        gamma_sign = -1;

    /* Finds the sign of the offscreen point in respect to the lines */
    int alpha_sign_offscreen = 1;
    int beta_sign_offscreen  = 1;
    int gamma_sign_offscreen = 1;

    if(signbit(f(x1, x2, y1, y2, OFFSCREEN[0], OFFSCREEN[1])))
        alpha_sign_offscreen = -1;
    if(signbit(f(x2, x0, y2, y0, OFFSCREEN[0], OFFSCREEN[1])))
        beta_sign_offscreen = -1;
    if(signbit(f(x0, x1, y0, y1, OFFSCREEN[0], OFFSCREEN[1])))
        gamma_sign_offscreen = -1;

    /* Computes the starting values for alph, beta and gamma, for the point
     * (xmin - 1, ymin - 1) so that it will be incremented to the correct
     * starting value in the first loop cycle.
     */
    int alpha = f(x1, x2, y1, y2, xmin - 1, ymin - 1) * alpha_sign;
    int beta  = f(x2, x0, y2, y0, xmin - 1, ymin - 1) * beta_sign;
    int gamma = f(x0, x1, y0, y1, xmin - 1, ymin - 1) * gamma_sign;

    for(int y = ymin; y <= ymax; y++) {
        alpha += f_incr(x1, x2, 0, 0, alpha_sign);
        beta += f_incr(x2, x0, 0, 0, beta_sign);
        gamma += f_incr(x0, x1, 0, 0, gamma_sign);

        /* Saves the current values to jump back to after the
         * x loop is finished
         */
        int prev_alpha = alpha;
        int prev_beta = beta;
        int prev_gamma = gamma;

        for(int x = xmin; x <= xmax; x++) {
            alpha += f_incr(0, 0, y1, y2, alpha_sign);
            beta += f_incr(0, 0, y2, y0, beta_sign);
            gamma += f_incr(0, 0, y0, y1, gamma_sign);


            if(alpha >= 0 && beta >= 0 && gamma >= 0) {
                /* Only draw if it's inside the triangle, or on the triangle edge
                 * if the triangle is on the side of the arbitrarily chosen
                 * offscreen point. This prevents shared edges being drawn twice
                 */
                if((alpha > 0 || alpha_sign * alpha_sign_offscreen > 0)
                && (beta  > 0 || beta_sign * beta_sign_offscreen > 0)
                && (gamma > 0 || gamma_sign * gamma_sign_offscreen > 0)) {
                    PutPixel(x, y, r, g, b);
                }
            }
        }
        alpha = prev_alpha;
        beta = prev_beta;
        gamma = prev_gamma;
    }
}
