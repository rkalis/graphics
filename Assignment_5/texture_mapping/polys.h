/* Computer Graphics
 *
 * Filename ........ polys.h
 * Description ..... Functions to manage lists of polygons (header file)
 * Date ............ 19.08.2008
 * Created by ...... Jurgen Sturm
 * Cleaned up by ... Paul Melis
 */

#ifndef POLYS_H
#define POLYS_H

#include <GL/gl.h>
#include "v3math.h"

#define MAX_VERTICES    4

typedef struct poly
{
    // Number of vertices
    int     points;

    // Vertices and their normals
    vec3    pts[MAX_VERTICES];
    vec3    normal[MAX_VERTICES];

    // Overall polygon color
    GLfloat color[4];

    // Texture identifier as set in a .obj file
    GLuint  texture_id;

    // Texture coordinates per vertex
    vec3    tcoord[MAX_VERTICES];
}
poly;


typedef struct polys
{
    /* size of the poly array "items" below */
    int     capacity;

    /* number of poly's stored, also index of the first free position
     * in the "items" array. If this number is equal to "capacity" above,
     * then this list is full */
    int     length;

    /* array of polygons, with length "capacity" of which only the first "length"
     * items will be in use */
    poly    *items;
}
polys;


/* Create an empty list of polys, initially reserving space for 'n' polys */
polys* CreatePolylist(int n);

/* Destroy a list of polygons */
void DestroyPolylist(polys *list);

/* poly's are structs of (point) arrays, so they are passed by value instead
 * of by reference (as do arrays) */
void AddPolyToPolylist(polys *list, poly p);

/* Append the items of 'to_append' to 'list' */
void AppendPolylist(polys *list, polys *to_append);

/* Copies the list of polys `list', calls AddPolytoPolylist() for each poly
 * in the given list. */
polys* CopyPolylist(polys *list);

/* Clear a list of polygons. Note: this does not de-allocate the item array */
void ClearPolylist(polys *list);

#endif
