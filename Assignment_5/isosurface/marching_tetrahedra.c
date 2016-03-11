/* Computer Graphics, Assignment, Volume rendering with cubes/points/isosurface
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

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "marching_tetrahedra.h"

int num_triangles;

/* Compute a linearly interpolated position where an isosurface cuts
   an edge between two vertices (p1 and p2), each with their own
   scalar value (v1 and v2) */

static vec3
interpolate_points(unsigned char isovalue, vec3 p1, vec3 p2, unsigned char v1, unsigned char v2)
{
    /* Initially, simply return the midpoint between p1 and p2.
       So no real interpolation is done yet */

    return v3_add(v3_multiply(p1, 0.5), v3_multiply(p2, 0.5));
}

int get_case_number(unsigned char isovalue, int v0, int v1, int v2, int v3){
    int case_no = 0;
    if(v0 > isovalue)
        case_no += 1;
    if(v1 > isovalue)
        case_no += 10;
    if(v2 > isovalue)
        case_no += 100;
    if(v3 > isovalue)
        case_no += 1000;
    return case_no;
}

void add_triangle(triangle **triangles, triangle t){
    *triangles[num_triangles*sizeof(triangle)] = t;
    num_triangles++;
}

/* Using the given iso-value generate triangles for the tetrahedron
   defined by corner vertices v0, v1, v2, v3 of cell c.

   Store the resulting triangles in the "triangles" array.

   Return the number of triangles created (either 0, 1, or 2).

   Note: the output array "triangles" should have space for at least
         2 triangles.
*/
int
generate_tetrahedron_triangles(triangle **triangles, unsigned char isovalue, cell c, int v0, int v1, int v2, int v3)
{

    int case_no = get_case_number(isovalue, v0, v1, v2, v3);
    triangle t;
    if (case_no == 1110 || 0001){

        t.p[0] = interpolate_points(isovalue, c.p[v0], c.p[v1], c.value[v0], c.value[v1]);
        t.p[1] = interpolate_points(isovalue, c.p[v0], c.p[v2], c.value[v0], c.value[v2]);
        t.p[2] = interpolate_points(isovalue, c.p[v0], c.p[v3], c.value[v0], c.value[v3]);

        add_triangle(triangles, t);
    }
    else if (case_no == 0010 || case_no == 1101){
        
        t.p[0] = interpolate_points(isovalue, c.p[v1], c.p[v0], c.value[v1], c.value[v0]);
        t.p[1] = interpolate_points(isovalue, c.p[v1], c.p[v2], c.value[v1], c.value[v2]);
        t.p[2] = interpolate_points(isovalue, c.p[v1], c.p[v3], c.value[v1], c.value[v3]);

        add_triangle(triangles, t);
    }
    else if (case_no == 0100 || case_no == 1011){
        
        t.p[0] = interpolate_points(isovalue, c.p[v2], c.p[v0], c.value[v2], c.value[v0]);
        t.p[1] = interpolate_points(isovalue, c.p[v2], c.p[v1], c.value[v2], c.value[v1]);
        t.p[2] = interpolate_points(isovalue, c.p[v2], c.p[v3], c.value[v2], c.value[v3]);

        add_triangle(triangles, t);
    }
    else if (case_no == 1000 || case_no == 0001){
        
        t.p[0] = interpolate_points(isovalue, c.p[v3], c.p[v0], c.value[v3], c.value[v0]);
        t.p[1] = interpolate_points(isovalue, c.p[v3], c.p[v1], c.value[v3], c.value[v1]);
        t.p[2] = interpolate_points(isovalue, c.p[v3], c.p[v2], c.value[v3], c.value[v2]);

        add_triangle(triangles, t);
    }
    else if (case_no == 0011 || case_no == 1100){

        t.p[0] = interpolate_points(isovalue, c.p[v0], c.p[v2], c.value[v0], c.value[v2]);
        t.p[1] = interpolate_points(isovalue, c.p[v0], c.p[v3], c.value[v0], c.value[v3]);
        t.p[2] = interpolate_points(isovalue, c.p[v1], c.p[v2], c.value[v1], c.value[v2]);

        add_triangle(triangles, t);

        triangle t2;

        t2.p[0] = interpolate_points(isovalue, c.p[v1], c.p[v2], c.value[v1], c.value[v2]);
        t2.p[1] = interpolate_points(isovalue, c.p[v1], c.p[v3], c.value[v1], c.value[v3]);
        t2.p[2] = interpolate_points(isovalue, c.p[v0], c.p[v3], c.value[v0], c.value[v3]);

        add_triangle(triangles, t2);
    }
    else if (case_no == 0101 || case_no == 1010){
        t.p[0] = interpolate_points(isovalue, c.p[v0], c.p[v1], c.value[v0], c.value[v1]);
        t.p[1] = interpolate_points(isovalue, c.p[v0], c.p[v3], c.value[v0], c.value[v3]);
        t.p[2] = interpolate_points(isovalue, c.p[v2], c.p[v3], c.value[v2], c.value[v3]);

        add_triangle(triangles, t);

        triangle t2;

        t2.p[0] = interpolate_points(isovalue, c.p[v2], c.p[v3], c.value[v2], c.value[v3]);
        t2.p[1] = interpolate_points(isovalue, c.p[v2], c.p[v1], c.value[v2], c.value[v1]);
        t2.p[2] = interpolate_points(isovalue, c.p[v0], c.p[v1], c.value[v0], c.value[v1]);

        add_triangle(triangles, t2);   
    }
    else if (case_no == 0110 || case_no == 1001){
        t.p[0] = interpolate_points(isovalue, c.p[v1], c.p[v3], c.value[v1], c.value[v3]);
        t.p[1] = interpolate_points(isovalue, c.p[v1], c.p[v0], c.value[v1], c.value[v0]);
        t.p[2] = interpolate_points(isovalue, c.p[v2], c.p[v0], c.value[v2], c.value[v0]);

        add_triangle(triangles, t);

        triangle t2;

        t2.p[0] = interpolate_points(isovalue, c.p[v2], c.p[v0], c.value[v2], c.value[v0]);
        t2.p[1] = interpolate_points(isovalue, c.p[v2], c.p[v3], c.value[v2], c.value[v3]);
        t2.p[2] = interpolate_points(isovalue, c.p[v1], c.p[v3], c.value[v1], c.value[v3]);

        add_triangle(triangles, t2);  
    }
    return num_triangles;
}


/* Generate triangles for a single cell for the given iso-value. This function
   should produce at most 6 * 2 triangles (for which the "triangles" array should
   have enough space).

   Use calls to generate_tetrahedron_triangles().

   Return the number of triangles produced
*/

int
generate_cell_triangles(triangle **triangles, cell c, unsigned char isovalue)
{
    *triangles = malloc(6*2*sizeof(triangle));
    printf("in cell_triangle\n");
    num_triangles = 0;
    generate_tetrahedron_triangles(triangles, isovalue, c, 0, 1, 3, 7); //T1
    generate_tetrahedron_triangles(triangles, isovalue, c, 0, 2, 6 ,7); //T2
    generate_tetrahedron_triangles(triangles, isovalue, c, 0, 1, 5, 7); //T3
    generate_tetrahedron_triangles(triangles, isovalue, c, 0, 2, 3, 7); //T4
    generate_tetrahedron_triangles(triangles, isovalue, c, 0, 4, 5, 7); //T5
    generate_tetrahedron_triangles(triangles, isovalue, c, 0, 4, 6, 7); //T6
    printf("out of cell_triangle\n");


    return num_triangles;
}
