/* Computer Graphics, Assignment 1, Bresenham's Midpoint Line-Algorithm
*
* Filename ........ mla.c
* Description ..... Midpoint Line Algorithm
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

#include "SDL.h"   
#include "init.h"

/*
* Midpoint Line Algorithm
*
* As you probably will have figured out, this is the part where you prove
* your programming skills. The code in the mla function should draw a direct 
* line between (x0,y0) and (x1,y1) in the specified color. 
* 
* Until now, the example code below draws only a horizontal line between
* (x0,y0) and (x1,y0) and a vertical line between (x1,y1).
* 
* And here the challenge begins..
*
* Good luck!
*
*
*/
void mla(SDL_Surface *s, int x0, int y0, int x1, int y1, Uint32 colour) {
    int ix,iy;
    int x,y;



    if (x1 < x0){
        int temp_x = x1;
        int temp_y = y1;
        x1 = x0;
        y1 = y0;
        x0 = temp_x;
        y0 = temp_y;
    }   
    printf("x0: %i, x1: %i, y0: %i, y1: %i\n", x0, x1, y0, y1);
    double d = (y0 - y1) * (x0 + 1) + (x1 - x0) * (y0 + 0.5) + (x0 * y1) - (x1 * y0);
    printf("d:  %d\n", d);

    // PutPixel(s,x0,y0,colour);
    // PutPixel(s,x1,y1,colour);

    // printf("Hello\n");



    // if(x1 > x0)
    //     ix = 1; 
    // else 
    //     ix = -1;

    // for(x=x0; x!=x1; x += ix){
    //     PutPixel(s,x,y0,colour);
    // }

    // if(y1 > y0) 
    //     iy = 1; 
    // else 
    //     iy = -1;

    // for(y=y0 ;y!=y1; y+=iy){
    //     PutPixel(s,x1,y,colour);
    // }

    return;
}

