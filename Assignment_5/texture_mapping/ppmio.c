/*
  By Robert G. Belleman, Section Computational Science,
  University of Amsterdam, Kruislaan 403, 1098 SJ Amsterdam,
  the Netherlands.

  Email : robbel@science.uva.nl
  URL   : http://www.science.uva.nl/~robbel/
  Tel.  : +31 20 525 7463 (secr)

  $Id: ppmio.c,v 1.1.1.1 2001/10/09 11:43:52 robbel Exp $
  $Log: ppmio.c,v $
  Revision 1.1.1.1  2001/10/09 11:43:52  robbel
  April 1997 CAVE demo developed by Robert G. Belleman in cooperation with
  ESI France.


*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char byte;

/*
  Read a ppm file. Very untested.
  One thing this function can't stand is more than one line of comment after
  the magic number (as xv tends to do).

  Returns a [width][height][3] pointer containing the red, green, blue
  components of a picture of size width x height. The area for this
  picture has been allocated using alloc3() and should therefore be freed
  by a call to free3(). The width and height of the picture are returned
  in the passed pointers.

  Returns NULL if anything failed.
*/
void *readppm(const char *filename, int *width, int *height)
{
    char buffer[80];            /* no line should be longer than 70 characters */
    FILE *fd;
    int maxcol;
    byte *area;
    int j;
#if defined(DEBUG)
    int dummy;
#endif

    if (!filename)
        return NULL;

    if (!(fd = fopen(filename, "r")))
    {
        perror(filename);
        return NULL;
    }

    /* read and check magic number */
    fscanf(fd, "%s\n", buffer);
    if (strcmp(buffer, "P6"))
        return NULL;            /* not a PPM raw file */

    /* skip comment line (if any) */

    fgets(buffer, 80, fd);
    if (buffer[0] == '#')
        fgets(buffer, 80, fd);

    /* read width, height and maximum color-component value */
    sscanf(buffer, "%d %d", width, height);
    fscanf(fd, "%s\n", buffer);
    maxcol = atoi(buffer);

#if defined(DEBUG)
    fprintf(stderr, "%s is %d x %d, max %d. Allocating %d bytes.\n",
            filename, *width, *height, maxcol, (*width) * (*height) * 3);
#endif

    /* allocate storage, then read data */
    if (!(area = (byte *) malloc((*width) * (*height) * 3 * sizeof(byte))))
    {
        fclose(fd);
        return NULL;
    }

    /* From the glTexImage2D man page on the pixels parameter:

    """The  first  element corresponds to the lower left corner of the texture image.  Subsequent elements progress left-to-right through the
       remaining texels in the lowest row of the texture image, and then in successively higher rows of the texture image.  The final element
       corresponds to the upper right corner of the texture image."""

    So we read line-by-line here and store the data bottom-top swapped. This way
    the data can be passed directly to glTexImage2D without getting a bottom-top swap.
    */

    for (j = *height-1; j >= 0; j--)
    {
#if defined(DEBUG)
        dummy =
#endif
            fread(area + j*(*width)*3, sizeof(byte), (*width) * 3, fd);

#if defined(DEBUG)
        fprintf(stderr, "%d bytes read. First 16 bytes:\n", dummy);
        for (dummy = 0; dummy < 16; dummy++)
            fprintf(stderr, "%02x ", area[dummy]);
        fprintf(stderr, "\n");
#endif
    }

    fclose(fd);
    return area;
}
