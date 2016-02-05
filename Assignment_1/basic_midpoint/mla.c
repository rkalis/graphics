/* Computer Graphics, Assignment 1, Bresenham's Midpoint Line-Algorithm
 *
 * Filename ........ mla.c
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

#include "SDL.h"   
#include "init.h"
#include "stdbool.h"

/* Midpoint Line Algorithm
 * Draws lines in all directions of a circle
 */
void mla(SDL_Surface *s, int x0, int y0, int x1, int y1, Uint32 colour) {

    PutPixel(s, x1, y1, colour);

    /* If the direction of line in the x direction is negative,
     * reverse the points.
     */
    if (x1 < x0){
        int temp_x = x1;
        int temp_y = y1;
        x1 = x0;
        y1 = y0;
        x0 = temp_x;
        y0 = temp_y;
    }

    /* If the direction of the line in the y direction is negative, 
     * switch y0 and y1.
     */
    bool negative = false;
    if((y1-y0)<0){
        negative = true;
        int temp = y1;
        y1 = y0;
        y0 = temp;
    }

    /* If the slope of the line in either the positive or negative direction is 
     * larger than 45 degrees, switch the x and y coordinates.
     */
    bool steep = false;
    if (abs(y1 - y0) > abs(x1 - x0)){
        steep = true;
    }
    if (steep){
        int temp_1 = x0;
        x0 = y0;
        y0 = temp_1;
        int temp_2 = x1;
        x1 = y1;
        y1 = temp_2;
    }

    int x,y;
    y = y0;
    
    /* Calculate the middle line point. */
    int d = (y0 - y1)*(x0 + 1) + (x1 - x0)*(y0 + 0.5) + (x0 * y1) - (x1 * y0);

    for (x = x0; x <= x1; x++){
        int temp_y;
        if(negative){
            temp_y = (y0 - (y - y1));
        }
        else{
            temp_y = y;
        }

        if(steep){
            PutPixel(s, temp_y, x,colour);
        }
        else{
            PutPixel(s, x, temp_y, colour);
        }
        if (d < 0){
            y++;
            d += (x1 - x0) + (y0 - y1);    
        }
        else{
            d += (y0 - y1);
        }
    }

    return;
}

