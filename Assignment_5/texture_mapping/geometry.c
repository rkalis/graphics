/* Computer Graphics, Assignment, Texture Mapping
 *
 * Filename ........ geometry.c
 * Description .....
 * Date ............ 22.11.2007
 * Created by ...... Paul Melis
 *
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

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "geometry.h"
#include "polys.h"

#ifndef M_PI
#define M_PI 3.141592653589793
#endif

static void
setSpherePoint(vec3 *p, vec3* n, vec3* t, int latitude, int longitude,
    double sx, double sy, double sz, double ox, double oy, double oz)
{
    double  dToR = M_PI / 180.0;
    double  len;

    // Set vertex position
    p->x = ox + sin(longitude * dToR) * cos(latitude * dToR) * sx;
    p->y = oy + sin(latitude * dToR) * sy;
    p->z = oz + cos(longitude * dToR) * cos(latitude * dToR) * sz;

    // Set texture coordinate
    t->x = 0.0;
    t->y = 0.0;

    // calculate normal, this actually doesn't take the sphere size
    // per axis into account, but should still be usable
    n->x = p->x - ox;
    n->y = p->y - oy;
    n->z = p->z - oz;

    len = n->x*n->x + n->y*n->y + n->z*n->z;
    n->x /= len;
    n->y /= len;
    n->z /= len;
}

void
createSphere(polys * list, double sx, double sy, double sz, double ox,
           double oy, double oz, double r, double g, double b)
{
    int     latitude, longitude;
    poly    p;

    // prepare poly variable, as the color values are the same for all generated polys
    p.points = 4;
    p.color[0] = r;
    p.color[1] = g;
    p.color[2] = b;
    p.color[3] = 0;

    // sweep over sphere's surface and generate polys
    for (latitude = -90; latitude < 90; latitude += 10)
    {
        for (longitude = 0; longitude < 360; longitude += 10)
        {
            setSpherePoint(&(p.pts[0]), &(p.normal[0]), &(p.tcoord[0]),
                latitude, longitude, sx, sy, sz, ox, oy, oz);
            setSpherePoint(&(p.pts[1]), &(p.normal[1]), &(p.tcoord[1]),
                latitude+10, longitude, sx, sy, sz, ox, oy, oz);
            setSpherePoint(&(p.pts[2]), &(p.normal[2]), &(p.tcoord[2]),
                latitude+10, longitude+10, sx, sy, sz, ox, oy, oz);
            setSpherePoint(&(p.pts[3]), &(p.normal[3]), &(p.tcoord[3]),
                latitude, longitude+10, sx, sy, sz, ox, oy, oz);

            AddPolyToPolylist(list, p);
        }
    }
}

// Calculate the coordinates, normal vector and texture coordinates for
// a hemisphere point at the given latitude and longitude (in degrees).
// The radius of the hemisphere is s, the center point (at the base of the
// hemisphere) is ox,oy,oz.
static void
setHemispherePoint(vec3 *p, vec3* n, vec3* t, int latitude, int longitude,
    double s, double ox, double oy, double oz)
{
    double  dToR = M_PI / 180.0;
    double  len;

    // Set vertex position
    p->x = ox + sin(longitude * dToR) * cos(latitude * dToR) * s;
    p->y = oy + sin(latitude * dToR) * s;
    p->z = oz + cos(longitude * dToR) * cos(latitude * dToR) * s;

    // Set texture coordinate
    t->x = 0.0;
    t->y = 0.0;

    // calculate normal
    n->x = p->x - ox;
    n->y = p->y - oy;
    n->z = p->z - oz;

    len = n->x*n->x + n->y*n->y + n->z*n->z;
    n->x /= len;
    n->y /= len;
    n->z /= len;
}

void
createHemisphere(polys * list, double s, double ox, double oy, double oz,
    double r, double g, double b)
{
    int     latitude, longitude;
    poly    p;

    // prepare poly, as these values are the same for all generated polys
    p.points = 4;
    p.color[0] = r;
    p.color[1] = g;
    p.color[2] = b;
    p.color[3] = 0;

    // sweep over sphere's surface and generate polys
    for (latitude = 0; latitude < 90; latitude += 10)
    {
        for (longitude = 0; longitude < 360; longitude += 10)
        {
            setHemispherePoint(&(p.pts[0]), &(p.normal[0]), &(p.tcoord[0]),
                latitude, longitude, s, ox, oy, oz);
            setHemispherePoint(&(p.pts[1]), &(p.normal[1]), &(p.tcoord[1]),
                latitude, longitude+10, s, ox, oy, oz);
            setHemispherePoint(&(p.pts[2]), &(p.normal[2]), &(p.tcoord[2]),
                latitude+10, longitude+10, s, ox, oy, oz);
            setHemispherePoint(&(p.pts[3]), &(p.normal[3]), &(p.tcoord[3]),
                latitude+10, longitude, s, ox, oy, oz);

            AddPolyToPolylist(list, p);
        }
    }
}

// Create a cylinder along the Y axis whose base center point is
// at (ox, oy, oz), having the given radius and height.
// Use the given color for the generated polygons.
void
createCylinder(polys * list, double radius, double height,
    double ox, double oy, double oz,
    double r, double g, double b)
{
    int     longitude;
    GLfloat x, z;
    GLfloat x2, z2;
    double  dToR = M_PI / 180.0;
    double  len;
    poly    p;
    int     i;

    // prepare poly datastructure, as these values are the same for all generated polys
    p.points = 4;
    p.color[0] = r;
    p.color[1] = g;
    p.color[2] = b;
    p.color[3] = 0;

    // sweep around cylinder axis
    for (longitude = 0; longitude < 360; longitude += 10)
    {
        x = ox + sin(longitude * dToR) * radius;
        z = oz + cos(longitude * dToR) * radius;

        x2 = ox + sin((longitude+10) * dToR) * radius;
        z2 = oz + cos((longitude+10) * dToR) * radius;

        p.pts[0].x = x;  p.pts[0].y = oy;           p.pts[0].z = z;
        p.pts[1].x = x;  p.pts[1].y = oy+height;    p.pts[1].z = z;
        p.pts[2].x = x2; p.pts[2].y = oy+height;    p.pts[2].z = z2;
        p.pts[3].x = x2; p.pts[3].y = oy;           p.pts[3].z = z2;

        for (i = 0; i < 4; i++)
        {
            p.normal[i].x = p.pts[i].x - ox;
            p.normal[i].y = 0.0;
            p.normal[i].z = p.pts[i].z - oz;

            len = p.normal[i].x*p.normal[i].x + p.normal[i].z*p.normal[i].z;
            p.normal[i].x /= len;
            p.normal[i].z /= len;

            // Set texture coordinate
            p.tcoord[i].x = p.tcoord[i].y = 0.0;
        }

        AddPolyToPolylist(list, p);
    }
}

// Read the vertex data for a single polygon and store it in 'p'.
// Called from loadPolygonalObject() after a 'p ...' line (+ color)
// has been read.
static void
readPolyVertices(poly *p, FILE *f, const vec3 *vertex_list, int num_vertices_to_read,
    int max_vertex_index, GLuint texture_name)
{
    char    line[1024];
    int     num_read;
    int     pidx;
    int     vidx;
    float   s, t;

    p->points = num_vertices_to_read;

    num_read = 0;
    pidx = 0;
    while (num_read < num_vertices_to_read)
    {
        if (fgets(line, 1024, f) == NULL)
        {
            printf("Unexpected end-of-file reading poly vertices!\n");
            exit(-1);
        }

        if (sscanf(line, "%d %f %f\n", &vidx, &s, &t) == 3)
        {
            if (vidx > max_vertex_index)
            {
                printf("While reading %d polygon vertices, line:\n", num_vertices_to_read);
                printf("%s\n", line);
                printf("Vertex index %d is out of range (should be <= %d)\n", vidx, max_vertex_index);
                exit(-1);
            }

            //printf("%d %f %f %f %f %f\n",  vidx,  nx,  ny,  nz,  s,  t);
            p->pts[pidx] = vertex_list[vidx];

            p->texture_id = texture_name;

            p->tcoord[pidx].x = s;
            p->tcoord[pidx].y = t;

            num_read++;
            pidx++;
        }
        else if (line[0] && line[0] != '#')
        {
            printf("Expected line declaring polygon vertex, got instead:\n");
            printf("%s\n", line);
            exit(-1);
        }

    }

}

// Read a polygonal object from a .OBJ file.
// Scale the input coordinates uniformly with a factor s followed by
// a translation (tx,ty,tz).
void
loadPolygonalObject(polys* list, const char *objfile, GLuint *texture_names,
    double s, double tx, double ty, double tz)
{
    FILE    *f;
    char    line[1024];
    float   x, y, z;
    vec3    *vertices;
    int     num_vertices;
    int     num_vertices_allocated;
    poly    p;
    float   colr, colg, colb;
    int     num_poly_vertices, texture;
    int     i;
    float   len;
    vec3    v, w, n;

    f = fopen(objfile, "rt");

    if (!f)
    {
        fprintf(stderr, "loadPolygonalObject(): Could not open file '%s' for reading!\n", objfile);
        exit(-1);
    }

    printf("Reading %s\n", objfile);

    num_vertices = 0;
    num_vertices_allocated = 1024;
    vertices = malloc(num_vertices_allocated * sizeof(vec3));

    while (fgets(line, 1024, f))
    {
        if (sscanf(line, "v %f %f %f\n", &x, &y, &z))
        {
            // vertex
            //printf("vertex: %f %f %f\n", x, y, z);

            // current vertex list is full, add more space
            if (num_vertices == num_vertices_allocated)
            {
                num_vertices_allocated = (int)(1.5 * num_vertices_allocated);
                vertices = realloc(vertices, num_vertices_allocated * sizeof(vec3));
                if (vertices == NULL)
                {
                    printf("No memory to hold vertices!\n");
                    exit(-1);
                }
            }

            // store vertex, after scaling and translation
            vertices[num_vertices].x = tx + s*x;
            vertices[num_vertices].y = ty + s*y;
            vertices[num_vertices].z = tz + s*z;

            num_vertices++;
        }
        else if (sscanf(line, "p %d %d\n", &num_poly_vertices, &texture))
        {
            if (num_poly_vertices < 3)
            {
                printf("In %s, line:\n", objfile);
                printf("%s\n", line);
                printf("Need at least 3 vertices to define a polygon!\n");
                exit(-1);
            }
            else if (num_poly_vertices > 4)
            {
                printf("In %s, line:\n", objfile);
                printf("%s\n", line);
                printf("Polygons may only consist of 3 or 4 vertices, sorry\n");
                printf("(this is due to OpenGL not handling concave polygons very well)\n");
                exit(-1);
            }

            // read polygon color
            if (fscanf(f, "%f %f %f\n", &colr, &colg, &colb) == EOF)
            {
                printf("Could not read polygon color!\n");
                exit(-1);
            }

            p.color[0] = colr;
            p.color[1] = colg;
            p.color[2] = colb;
            p.color[3] = 0.0;

            // read vertices making up poly
            readPolyVertices(&p, f, vertices, num_poly_vertices, num_vertices-1, texture_names[texture]);

            // Use first 3 polygon vertices to calculate the polygon normal vector
            // (i.e. assumes polygon is planar)

            v.x = p.pts[2].x - p.pts[1].x;
            v.y = p.pts[2].y - p.pts[1].y;
            v.z = p.pts[2].z - p.pts[1].z;

            w.x = p.pts[0].x - p.pts[1].x;
            w.y = p.pts[0].y - p.pts[1].y;
            w.z = p.pts[0].z - p.pts[1].z;

            n.x = v.y * w.z - v.z * w.y;
            n.y = v.z * w.x - v.x * w.z;
            n.z = v.x * w.y - v.y * w.x;

            len = sqrt(n.x * n.x + n.y * n.y + n.z * n.z);
            n.x /= len;
            n.y /= len;
            n.z /= len;

            // Set vertex normals to polygon normal

            for (i = 0; i < num_poly_vertices; i++)
                p.normal[i] = n;

            // Add polygon

            AddPolyToPolylist(list, p);
        }
    }

    //printf("%d vertices read\n", num_vertices);

    fclose(f);

    free(vertices);
}

