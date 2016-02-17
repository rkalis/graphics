/* Computer Graphics, Assignment, Bezier curves
 * Filename ........ bezier.c
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

#include <math.h>
#include "bezier.h"
#include <stdio.h>

/* Returns the factorial of given number n */
int factorial(int n) {
    int fact = 1;
    for (int i = 1; i <= n; i++)
        fact = fact * i;
    return fact;
}

/* Given a Bezier curve defined by the 'num_points' control points
 * in 'p' computes the position of the point on the curve for parameter
 * value 'u'.
 * Returns the x and y values of the point by setting *x and *y, respectively.
 */
void evaluate_bezier_curve(float *x, float *y, control_point p[], int num_points, float u) {
    *x = 0.0;
    *y = 0.0;
    int n = num_points - 1;
    for(int k = 0; k <= n; k++) {
        float coef = (factorial(n)) / (factorial(k) * factorial(n - k));
        float sum = pow(u, k) * pow((1 - u), (n - k));
        *x += p[k].x * coef * sum;
        *y += p[k].y * coef * sum;
    }
}

/* Draws a Bezier curve defined by the control points in p[], which
 * will contain 'num_points' points.
 * Draws the line segments you compute directly on the screen
 * as a single GL_LINE_STRIP.
 * The 'num_segments' parameter determines the "discretization" of the Bezier
 * curve and is the number of straight line segments that should be used
 * to approximate the curve.
 * Calls evaluate_bezier_curve() to compute the necessary points on
 * the curve.
 */
void draw_bezier_curve(int num_segments, control_point p[], int num_points) {
    float x, y;
    glBegin(GL_LINE_STRIP);
    for(int i = 0; i <= num_segments; i++) {
        evaluate_bezier_curve(&x, &y, p, num_points, (float) i / (float) num_segments);
        glVertex2f(x, y);
    }
    glEnd();
    printf("dingen: ");
    printf("%d, %d\n", sizeof(p), sizeof(control_point));
}

/* Finds the intersection of a cubic Bezier curve with the line X=x.
 * It divides the bezier curve up into a lot of smaller pieces, and checks
 * if the piece intersects the line X=x. If it does, it sets *7 to
 * the average y value of the intersecting piece.
 * This isn't the most optimised strategy, but it is the simplest.
 * Returns 1 if an intersection was found and places the corresponding y
 * value in *y.
 * Returns 0 if no intersection exists.
*/
int intersect_cubic_bezier_curve(float *y, control_point p[], float x) {
    float x_old;
    float x_new;
    float y_old;
    float y_new;
    int max_i = 1000;
    int num_points = 4;
    evaluate_bezier_curve(&x_old, &y_old, p, num_points, 0);
    for(int i = 1; i <= max_i; i++) {
        float u = (float) i / (float) max_i;
        evaluate_bezier_curve(&x_new, &y_new, p, num_points, u);
        if(x_old < x && x_new > x) {
            y = (y_old + y_new) / 2;
            return 1;
        }
    }
    return 0;
}
