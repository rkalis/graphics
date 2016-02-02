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

/*
 * Rasterize a single triangle.
 * The triangle is specified by its corner coordinates
 * (x0,y0), (x1,y1) and (x2,y2).
 * The triangle is drawn in color (r,g,b).
 */

/* Defines the offscreen point for drawing edges */ 
int OFFSCREEN[] = {-1, -1};

float max_3(float a, float b, float c) {
    float max_a_b = fmax(a, b);
    return fmax(max_a_b, c);
}

float min_3(float a, float b, float c) {
    float min_a_b = fmin(a, b);
    return fmin(min_a_b, c);
}

float f(float x0, float x1, float y0, float y1, int x, int y) {
    return (y0 - y1) * x + (x1 - x0) * y + x0 * y1 - x1 * y0;
}

void draw_triangle(float x0, float y0, float x1, float y1, float x2, float y2,
                   byte r, byte g, byte b) {
    int xmin = (int) floor(min_3(x0, x1, x2));
    int xmax = (int) ceil(max_3(x0, x1, x2));
    int ymin = (int) floor(min_3(y0, y1, y2));
    int ymax = (int) ceil(max_3(y0, y1, y2));

    float alpha, beta, gamma;

    /* Equations for the points in respect to the lines of the triangle */
    float f_alpha = f(x1, x2, y1, y2, x0, y0);
    float f_beta = f(x2, x0, y2, y0, x1, y1);
    float f_gamma = f(x0, x1, y0, y1, x2, y2);

    /* Equations for the offscreen points in respect to the 
     * lines of the triangle */
    float f_alpha_offscreen = f(x1, x2, y1, y2, OFFSCREEN[0], OFFSCREEN[1]);
    float f_beta_offscreen  = f(x2, x0, y2, y0, OFFSCREEN[0], OFFSCREEN[1]);
    float f_gamma_offscreen = f(x0, x1, y0, y1, OFFSCREEN[0], OFFSCREEN[1]);

    for(int y = ymin; y <= ymax; y++) {
        for(int x = xmin; x <= xmax; x++) {
            alpha = f(x1, x2, y1, y2, x, y) / f_alpha;
            beta = f(x2, x0, y2, y0, x, y) / f_beta;
            gamma = f(x0, x1, y0, y1, x, y) / f_gamma;

            if(alpha >= 0 && beta >= 0 && gamma >= 0) {
                if((alpha > 0 || f_alpha * f_alpha_offscreen > 0)
                && (beta > 0 || f_beta * f_beta_offscreen > 0)
                && (gamma > 0 || f_gamma * f_gamma_offscreen > 0)) {
                    PutPixel(x, y, r, g, b);
                    // PutPixel(x, y, 255 * alpha, 255 * beta, 255 * gamma);
                }
            }
        }
    }
}

void draw_triangle_optimized(float x0, float y0, float x1, float y1, float x2, float y2,
                             byte r, byte g, byte b) {
    draw_triangle(x0, y0, x1, y1, x2, y2, r, g, b);
}
