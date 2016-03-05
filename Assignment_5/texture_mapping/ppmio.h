/*
  By Robert G. Belleman, Section Computational Science,
  University of Amsterdam, Kruislaan 403, 1098 SJ Amsterdam,
  the Netherlands.

  Email : robbel@science.uva.nl
  URL   : http://www.science.uva.nl/~robbel/
  Tel.  : +31 20 525 7463 (secr)

  $Id: ppmio.h,v 1.1.1.1 2001/10/09 11:43:52 robbel Exp $
  $Log: ppmio.h,v $
  Revision 1.1.1.1  2001/10/09 11:43:52  robbel
  April 1997 CAVE demo developed by Robert G. Belleman in cooperation with
  ESI France.
*/

#ifndef _PPMIO_H
#define _PPMIO_H

void *readppm(const char *filename, int *width, int *height);

#endif

/* _PPMIO_H */
