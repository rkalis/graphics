#ifndef VOLUME_H
#define VOLUME_H

#include "v3math.h"

/* A cell in the volume dataset, consisting of 8 neighbouring datapoints;
   p[] contains the corner positions, value[] the associated scalar values */
typedef struct
{
    vec3    p[8];
    vec3    n[8];       // Note: we experiment with normals in the solution
    double  value[8];
}
cell;

/* The data points in the volume dataset, stored as a one-dimensional array */
extern unsigned char    *volume;

/* The dimensions of the volume dataset in number of voxels in each
   dimension*/
extern int                nx, ny, nz;

/* The size of a voxel for each dimension */
extern float              sizex, sizey, sizez;

/* Utility function to convert the index of a datapoint
   into an index in the volume array above */
int voxel2idx(int i, int j, int k);

/* Extract a cell from the volume, so that datapoint 0 of the
   cell corresponds to voxel (i, j, k) */
cell get_cell(int i, int j, int k);

/* Utility function to read a volume dataset from a file.
   This will store the data in the "volume" array and update the dimension
   and size values. */
void read_volume(const char *fname);

#endif
