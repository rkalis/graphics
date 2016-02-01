/* Computer Graphics, Assignment 1, Bresenham's Midpoint Line-Algorithm
 *
 * Filename ........ main.c
 * Description ..... main program (opens window, draws figure)
 * Created by ...... Jurgen Sturm 
 *
 */
//test 
#include "SDL.h"   
#include "init.h"

SDL_Surface *screen;

int main(void) {

screen = InitialiseScreen(400, 300);

  ReportPixelFormat(screen);

  DrawFigure(screen);

  WaitForEvent();

  SDL_Quit();

  return(0);
}
