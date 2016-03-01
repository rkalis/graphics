#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include "rply.h"
#include "plymodel.h"
#include "v3math.h"

static int  vertex_cb(p_ply_argument argument);
static int  normal_cb(p_ply_argument argument);
static int  face_cb(p_ply_argument argument);

static void dump(void);

static int  vertex_element_idx, normal_element_idx, face_element_idx;

void
read_ply_model(const char *fname)
{
    int     i, j, k;

    p_ply   ply = ply_open(fname, NULL);

    // XXX handle better
    if (!ply) return;
    if (!ply_read_header(ply)) return;

    ply_num_vertices = ply_set_read_cb(ply, "vertex", "x", vertex_cb, NULL, 0);
    ply_set_read_cb(ply, "vertex", "y", vertex_cb, NULL, 0);
    ply_set_read_cb(ply, "vertex", "z", vertex_cb, NULL, 0);

    ply_set_read_cb(ply, "vertex", "nx", normal_cb, NULL, 0);
    ply_set_read_cb(ply, "vertex", "ny", normal_cb, NULL, 0);
    ply_set_read_cb(ply, "vertex", "nz", normal_cb, NULL, 0);

    int ply_num_faces = ply_set_read_cb(ply, "face", "vertex_indices", face_cb, NULL, 0);

    fprintf(stderr, "Reading %s\n", fname);

    ply_vertices = malloc(ply_num_vertices * sizeof(vec3));
    ply_normals = malloc(ply_num_vertices * sizeof(vec3));

    // XXX this will overallocate if some of the faces aren't triangles
    ply_triangles = malloc(ply_num_faces * sizeof(triangle));

    // will be updated by the callbacks
    ply_num_vertices = 0;
    ply_num_normals = 0;
    ply_num_triangles = 0;

    vertex_element_idx = 0;
    normal_element_idx = 0;
    face_element_idx = 0;

    // XXX
    if (!ply_read(ply))
    {
        fprintf(stderr, "\nFATAL: Could not read .ply file %s!\n", fname);
        exit(-1);
    }

    fprintf(stderr, "... %d vertices, %d normals, %d triangles\n",
        ply_num_vertices, ply_num_normals, ply_num_triangles);

    // Compute triangle normals

    fprintf(stderr, "... Computing triangle normals ... ");

    for (int t = 0; t < ply_num_triangles; t++)
    {
        i = ply_triangles[t].v[0];
        j = ply_triangles[t].v[1];
        k = ply_triangles[t].v[2];

        // Calculate normal and store it with the triangle
        ply_triangles[t].n = v3_normalize(
            v3_crossprod(
                v3_subtract(ply_vertices[j], ply_vertices[i]),
                v3_subtract(ply_vertices[k], ply_vertices[i])));
    }

    if (ply_num_normals == ply_num_vertices)
    {
        // Simply copy triangle vertex normals
        for (int t = 0; t < ply_num_triangles; t++)
        {
            for (i = 0; i < 3; i++)
                ply_triangles[t].vn[i] = ply_normals[ply_triangles[t].v[i]];
        }
    }
    else
    {
        // Incorrect number of vertex normals provided.
        // Set triangle's vertex normals to triangle normal, i.e.
        // flat shading
        for (int t = 0; t < ply_num_triangles; t++)
        {
            for (i = 0; i < 3; i++)
                ply_triangles[t].vn[i] = ply_triangles[t].n;
        }
    }

    fprintf(stderr, "done\n");

    // all done...

    ply_close(ply);
}

int
vertex_cb(p_ply_argument argument)
{
    double      v = ply_get_argument_value(argument);

    switch (vertex_element_idx)
    {
        case 0:
            ply_vertices[ply_num_vertices].x = v;
            break;
        case 1:
            ply_vertices[ply_num_vertices].y = v;
            break;
        case 2:
            ply_vertices[ply_num_vertices].z = v;
            break;
    }

    if (vertex_element_idx == 2)
    {
        ply_num_vertices++;
        vertex_element_idx = 0;
    }
    else
        vertex_element_idx++;

    return 1;
}

int
normal_cb(p_ply_argument argument)
{
    double      v = ply_get_argument_value(argument);

    switch (normal_element_idx)
    {
        case 0:
            ply_normals[ply_num_normals].x = v;
            break;
        case 1:
            ply_normals[ply_num_normals].y = v;
            break;
        case 2:
            ply_normals[ply_num_normals].z = v;
            break;
    }

    if (normal_element_idx == 2)
    {
        ply_num_normals++;
        normal_element_idx = 0;
    }
    else
        normal_element_idx++;

    return 1;
}

int
face_cb(p_ply_argument argument)
{
    int32        length, value_index;

    ply_get_argument_property(argument, NULL, &length, &value_index);

    // Skip any polygon that isn't a triangle
    if (length == 3 && value_index >= 0)
    {
        int idx = (int)ply_get_argument_value(argument);
        ply_triangles[ply_num_triangles].v[face_element_idx] = idx;

        if (face_element_idx == 2)
        {
            ply_num_triangles++;
            face_element_idx = 0;
        }
        else
            face_element_idx++;
    }

    return 1;
}

static void
dump(void)
{
    int i;

    for (i = 0; i < ply_num_vertices; i++)
    {
        if (ply_num_normals == ply_num_vertices)
        {
            printf("[%d] %.6f, %.6f, %.6f  (%.6f, %.6f, %.6f)\n", i,
                ply_vertices[i].x, ply_vertices[i].y, ply_vertices[i].z,
                ply_normals[i].x, ply_normals[i].y, ply_normals[i].z);
        }
        else
        {
            printf("[%d] %.6f, %.6f, %.6f\n", i, ply_vertices[i].x,
                ply_vertices[i].y, ply_vertices[i].z);
        }
    }

    for (i = 0; i < ply_num_triangles; i++)
    {
        printf("[%d] %d %d %d\n", i, ply_triangles[i].v[0],
            ply_triangles[i].v[1], ply_triangles[i].v[2]);
    }
}

