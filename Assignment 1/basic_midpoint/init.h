/* Computer Graphics, Assignment 1, Bresenham's Midpoint Line-Algorithm
 *
 * Filename ........ init.h
 * Description ..... Header file for init.c
 * Created by ...... Jurgen Sturm 
 *
 */

#ifndef JS_INIT_H
#define JS_INIT_H

/* ANSI C/ISO C89 does not specify this constant (?) */
#ifndef M_PI
#define M_PI           3.14159265358979323846  /* pi */
#endif

/* round(), roundf(), and roundl() ARE provided by the math library, but they
 * are not prototyped.
 */
extern float roundf(float x);

extern SDL_Surface *screen;

SDL_Surface* InitialiseScreen(int width, int height);
void WaitForEvent(void);
void ReportPixelFormat(SDL_Surface *screen);
void PutPixel(SDL_Surface *screen, int x, int y, Uint32 pixel);
void DrawFigure(SDL_Surface *screen);
void DrawFigureGL(SDL_Surface *screen);

#endif /* JS_INIT_H */
