/* Computer Graphics, Assignment 1, Bresenham's Midpoint Line-Algorithm
 *
 * Filename ........ init.c
 * Description ..... SDL Surface initialization, Pixel operations
 * Created by ...... Jurgen Sturm 
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "SDL.h"   
#include "init.h"
#include "mla.h"

void PutPixel(SDL_Surface *surface, int x, int y, Uint32 pixel);

void ReportPixelFormat(SDL_Surface *s) {
  SDL_PixelFormat *f=s->format;
  printf("screen(%d,%d) %d bits/%d bytes per pixel\n",
         s->w, s->h, f->BitsPerPixel,f->BytesPerPixel );

  return;
}

void PutPixel(SDL_Surface *surface, int x, int y, Uint32 pixel) {
  int bpp = surface->format->BytesPerPixel;
  /* Here p is the address to the pixel we want to set */
  Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

  switch(bpp) {
  case 1:
    *p = pixel;
    break;

  case 2:
    *(Uint16 *)p = pixel;
    break;

  case 3:
    if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
      p[0] = (pixel >> 16) & 0xff;
      p[1] = (pixel >> 8) & 0xff;
      p[2] = pixel & 0xff;
    } else {
      p[0] = pixel & 0xff;
      p[1] = (pixel >> 8) & 0xff;
      p[2] = (pixel >> 16) & 0xff;
    }
    break;

  case 4:
    *(Uint32 *)p = pixel;
   break;
  }
}


void DrawFigure(SDL_Surface *screen) {
  int i, mid_x=screen->w/2, mid_y=screen->h/2, size=100;
  Uint32 colour;


  if(SDL_MUSTLOCK(screen)) {
    if(SDL_LockSurface(screen)) {
      printf("Unable to lock surface: %s\n",SDL_GetError());
    }
  }

  for(i=0; i<32; i++) {
    /* Set drawing colour */
    switch((i>>2) % 4) {
      case 0:
        colour=SDL_MapRGB(screen->format,0xFF,0,0);
        break;
      case 1:
        colour=SDL_MapRGB(screen->format,0,0xFF,0);
        break;
      case 2:
        colour=SDL_MapRGB(screen->format,0,0,0xFF);
        break;
      case 3:
        colour=SDL_MapRGB(screen->format,0xFF,0xFF,0xFF);
        break;
    }

    /* draw a line */
    mla(screen, mid_x, mid_y, 
        (int)roundf(size*cos(2*M_PI/32*i))+mid_x, 
        (int)roundf(size*sin(2*M_PI/32*i))+mid_y, colour);
  }

  /* It would be more efficient to call SDL_UpdateRect(), but I do not really
   * care.
   */
  SDL_Flip(screen);

  if(SDL_MUSTLOCK(screen)) {
    SDL_UnlockSurface(screen);
  }

  return;
}


SDL_Surface* InitialiseScreen(int w, int h) {
  SDL_Surface *screen;
  if(SDL_Init(SDL_INIT_VIDEO) == -1) {
    printf("Unable to initialise SDL video subsystem: %s", SDL_GetError());
    exit(-1);
  }

  atexit(SDL_Quit);

  /* Create a `screen' (surface) to render into 
   * `SDL_DOUBLEBUF' flag is hack(ish)
   */
  screen=SDL_SetVideoMode(w, h, 0, SDL_HWSURFACE);
  if(screen == NULL) {
    printf("Unable to create SDL surface in video memory: `%s', ",
           SDL_GetError());
    printf("trying software emulation instead.\n");

    screen=SDL_SetVideoMode(w, h, 0, SDL_SWSURFACE|SDL_DOUBLEBUF);
    if(screen == NULL) {
      printf("Unable to create any SDL surface: `%s', giving up.\n",
             SDL_GetError());

      exit(-1);
    }
  }
  else {
    printf("screen->flags: %u\n", screen->flags);
    if (0 == (screen->flags & SDL_HWSURFACE))
      printf("Can't get hardware surface\n");
    if (0 == (screen->flags & SDL_SWSURFACE))
      printf("Can't get software surface\n");
    if (0 == (screen->flags & SDL_DOUBLEBUF))
      printf("Can't get double-buffered surface\n");

  }

  return(screen);
}


void WaitForEvent(void) {
  int quit=0;

  printf("Press anykey to quit\n");

  while(! quit) {
    SDL_Event event;
    int res=SDL_WaitEvent(&event);
    if(res == 0) {
      printf("Waiting for an event failed: %s\n", SDL_GetError());
      exit(-1);
    }

    switch(event.type) {
    case SDL_KEYDOWN:
      quit=1;
      break;
    case SDL_QUIT:
      quit=1;
      break;
    }
  }

  return;
}


