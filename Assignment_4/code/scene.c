#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "plymodel.h"
#include "bvh.h"

int         scene_num_triangles, scene_num_vertices;
triangle    *scene_triangles=NULL;
vec3        *scene_vertices=NULL;

int     	scene_num_spheres;
sphere  	*scene_spheres=NULL;

int         scene_num_lights;
light       *scene_lights=NULL;
float       scene_ambient_light = 0.05;

vec3    	scene_background_color = { 1.0, 1.0, 1.0 };

vec3        scene_camera_position, scene_camera_lookat;

void
read_scene(const char *fname)
{
	FILE 	*f;
	char	line[256];
	int		i;
	float	x, y, z, r;
	char	s[256];
	int		material;

	f = fopen(fname, "rt");

	if (!f)
	{
        fprintf(stderr, "\nFATAL: Could not open scene file %s!\n", fname);
        exit(-1);
    }

    printf("Reading scene %s\n", fname);

	scene_num_triangles = scene_num_vertices = 0;
	scene_num_spheres = 0;

	material = 0;

	// Bare bones, but usable, parsing
	fgets(line, 255, f);

	while (!feof(f))
	{
        if (line[0] == '/' && line[1] == '/')
        {
            // Comment, skip
        }
        else if (sscanf(line, "material %d\n", &material) == 1)
		{
			// Nothing to do, value already stored...
		}
        else if (sscanf(line, "light %f %f %f %f\n", &x, &y, &z, &r) == 4)
        {
			scene_lights = realloc(scene_lights, (scene_num_lights+1)*sizeof(light));
			scene_lights[scene_num_lights].position.x = x;
			scene_lights[scene_num_lights].position.y = y;
			scene_lights[scene_num_lights].position.z = z;
			scene_lights[scene_num_lights].intensity = r;
			scene_num_lights++;
        }
		else if (sscanf(line, "sphere %f %f %f %f\n", &x, &y, &z, &r) == 4)
		{
			scene_spheres = realloc(scene_spheres, (scene_num_spheres+1)*sizeof(sphere));
			scene_spheres[scene_num_spheres].center.x = x;
			scene_spheres[scene_num_spheres].center.y = y;
			scene_spheres[scene_num_spheres].center.z = z;
			scene_spheres[scene_num_spheres].radius = r;
			scene_spheres[scene_num_spheres].material = material;
			scene_num_spheres++;
		}
		else if (sscanf(line, "ply_file %s\n", s) == 1)
		{
            // Read triangle model from a .ply file, add the triangles
            // in the file to the global list of triangles in the scene

            printf("[%s]\n", s);

			read_ply_model(s);

			// Allocate memory to hold the new vertices and triangles

			scene_vertices = realloc(scene_vertices,
                (scene_num_vertices + ply_num_vertices)*sizeof(vec3));
			scene_triangles = realloc(scene_triangles,
                (scene_num_triangles + ply_num_triangles)*sizeof(triangle));

			// Copy the vertices
			memcpy(scene_vertices + scene_num_vertices,
                ply_vertices, ply_num_vertices*sizeof(vec3));

			// Copy the triangles, AND RENUMBER THEIR VERTEX INDICES
            // (as we're going to add the model triangles and vertices
            // to a scene-wide list of triangles and vertices)
			for (i = 0; i < ply_num_triangles; i++)
			{
				scene_triangles[scene_num_triangles+i] = ply_triangles[i];

				// Renumber the vertex indices
				scene_triangles[scene_num_triangles+i].v[0] += scene_num_vertices;
				scene_triangles[scene_num_triangles+i].v[1] += scene_num_vertices;
				scene_triangles[scene_num_triangles+i].v[2] += scene_num_vertices;

				// Assign the material
				scene_triangles[scene_num_triangles+i].material = material;
			}

			// Update the total numer of vertices in the scene
			scene_num_vertices += ply_num_vertices;
			scene_num_triangles += ply_num_triangles;
		}
        else if (line[0] != '\n')
        {
            fprintf(stderr, "Ignoring unknown line '%s'\n", line);
        }

		fgets(line, 255, f);
	}

	fclose(f);

    // Build the BVH for the triangles read
    bvh_build();
}
