#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "polys.h"

// Create a sphere centered at (ox, oy, oz), having sizes
// sx, sy and sz in X, Y and Z respectively. Use the given color.
void
createSphere(polys * list, double sx, double sy, double sz, double ox,
             double oy, double oz, double r, double g, double b);

// Create a hemisphere whose base point is at (ox, oy, oz), having radius s.
// Use the given color.
void
createHemisphere(polys * list, double s, double ox, double oy, double oz,
    double r, double g, double b);

// Create a cylinder along the Y axis whose base center point is
// at (ox, oy, oz), having the given radius and height.
// Use the given color for the generated polygons.
void
createCylinder(polys * list, double radius, double height,
    double ox, double oy, double oz,
    double r, double g, double b);

// Read a polygonal object from a .OBJ file.
// Scale the input coordinates uniformly with a factor s followed by
// a translation (tx,ty,tz).
void
loadPolygonalObject(polys * list, const char *objfile, GLuint *texture_names,
                    double s, double tx, double ty, double tz);


#endif
