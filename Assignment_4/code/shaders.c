/* Computer Graphics and Game Technology, Assignment Ray-tracing
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
#include "shaders.h"
#include "perlin.h"
#include "v3math.h"
#include "intersection.h"
#include "scene.h"
#include "quat.h"
#include "constants.h"

/* Returns red, used to create a silhoutte of an object */
vec3 shade_constant(intersection_point ip) {
    return v3_create(1, 0, 0);
}

/* Uses a simple matte surface shader with shadows */
vec3 shade_matte(intersection_point ip) {
    /* Start with ambient light */
    float contribution = scene_ambient_light;

    for(int i = 0; i < scene_num_lights; i++) {
        /* Create the vector between the ip and the light and normalize it */
        vec3 light_vector = v3_subtract(scene_lights[i].position, ip.p);
        light_vector = v3_normalize(light_vector);

        /* Check for shadows from two different origins to prevent self-shading
         * on the balls */
        vec3 ray_origin1 = v3_add(ip.p, v3_create(0.001, 0.001, 0.001));
        vec3 ray_origin2 = v3_add(ip.p, v3_create(-0.001, -0.001, -0.001));
        int shadow = shadow_check(ray_origin1, light_vector);
        shadow += shadow_check(ray_origin2, light_vector);

        float light_contribution = v3_dotprod(ip.n, light_vector) 
                                 * scene_lights[i].intensity;

        /* If there is light contribution and the ip isn't in the shadow
         * add the contribution of the light to the overall contribution */
        if(light_contribution > 0 &&  shadow < 2) {
            contribution += light_contribution;
        }
    }

    return v3_create(contribution, contribution, contribution);
}

/* Adds a specular component to the matte shader using the formula:
 * cf = cd * (Ia + Kd * sum_i{Ii * max(0, n . li)}
 *    + cs * Ks * sum_i{Ii * (n . h)^alpha} 
 */
vec3 shade_blinn_phong(intersection_point ip) {
    int alpha = 50;

    float kd = 0.8;
    vec3 cd = v3_create(1, 0, 0);

    float ks = 0.5;
    vec3 cs = v3_create(1, 1, 1);

    float diffuse_component = 0;
    float specular_component = 0;

    for(int i = 0; i < scene_num_lights; i++) {
        /* Create the vector between the ip and the light and normalize it */
        vec3 light_vector = v3_subtract(scene_lights[i].position, ip.p);
        light_vector = v3_normalize(light_vector);

        /* Create the vector in the middle between the camera and the light
         * and normalize it */
        vec3 h = v3_add(scene_lights[i].position, ip.i);
        h = v3_normalize(h);

        /* Check for shadows from two different origins to prevent self-shading
         * on the balls */
        vec3 ray_origin1 = v3_add(ip.p, v3_create(0.001, 0.001, 0.001));
        vec3 ray_origin2 = v3_add(ip.p, v3_create(-0.001, -0.001, -0.001));
        int shadow = shadow_check(ray_origin1, light_vector);
        shadow += shadow_check(ray_origin2, light_vector);

        /* Compute light contributions */
        float diffuse_contribution  = v3_dotprod(ip.n, light_vector)
                                    * scene_lights[i].intensity;
        float specular_contribution = pow(v3_dotprod(ip.n, h), alpha)
                                    * scene_lights[i].intensity;

        /* If there is light contribution and the ip isn't in the shadow
         * add the contribution of the light to the corresponding component */
        if(shadow < 2) {
            if(diffuse_contribution > 0) {
                diffuse_component += diffuse_contribution;
            }
            specular_component += specular_contribution;
        }
    }

    diffuse_component  *= kd;
    diffuse_component  += scene_ambient_light;

    specular_component *= ks;

    return v3_add(v3_multiply(cd, diffuse_component), 
                  v3_multiply(cs, specular_component));

}

vec3 shade_reflection(intersection_point ip) {
    int alpha = 50;

    float kd = 0.8;
    vec3 cd = v3_create(1, 1, 1);

    float ks = 0.5;
    vec3 cs = v3_create(1, 1, 1);

    float diffuse_component = 0;
    float specular_component = 0;

    for(int i = 0; i < scene_num_lights; i++) {
        /* Create the vector between the ip and the light and normalize it */
        vec3 light_vector = v3_subtract(scene_lights[i].position, ip.p);
        light_vector = v3_normalize(light_vector);

        /* Create the vector in the middle between the camera and the light
         * and normalize it */
        vec3 h = v3_add(scene_lights[i].position, ip.i);
        h = v3_normalize(h);

        /* Check for shadows from two different origins to prevent self-shading
         * on the balls */
        vec3 ray_origin1 = v3_add(ip.p, v3_create(0.001, 0.001, 0.001));
        vec3 ray_origin2 = v3_add(ip.p, v3_create(-0.001, -0.001, -0.001));
        int shadow = shadow_check(ray_origin1, light_vector);
        shadow += shadow_check(ray_origin2, light_vector);

        /* Compute light contributions */
        float diffuse_contribution  = v3_dotprod(ip.n, light_vector)
                                    * scene_lights[i].intensity;
        float specular_contribution = pow(v3_dotprod(ip.n, h), alpha)
                                    * scene_lights[i].intensity;

        /* If there is light contribution and the ip isn't in the shadow
         * add the contribution of the light to the corresponding component */
        if(shadow < 2) {
            if(diffuse_contribution > 0) {
                diffuse_component += diffuse_contribution;
            }
            specular_component += specular_contribution;
        }
    }

    diffuse_component  *= kd;
    diffuse_component  += scene_ambient_light;

    specular_component *= ks;

    /* Computes the base colour of the ip */
    vec3 diffuse_colour = v3_multiply(cd, diffuse_component);
    vec3 specular_colour = v3_multiply(cs, specular_component);
    vec3 colour = v3_add(diffuse_colour, specular_colour);

    /* Computes the direction of the reflected ray */
    vec3 reflected_ray = v3_subtract(
                         v3_multiply(ip.n, 2 * v3_dotprod(ip.n, ip.i)), ip.i);

    /* Computes the reflection colour from two different ray origins to prevent
     * speckled balls */
    vec3 ray_origin1 = v3_add(ip.p, v3_create(0.001, 0.001, 0.001));
    vec3 ray_origin2 = v3_add(ip.p, v3_create(-0.001, -0.001, -0.001));
    vec3 reflection1 = ray_color(ip.ray_level + 1, ray_origin1, reflected_ray);
    vec3 reflection2 = ray_color(ip.ray_level + 1, ray_origin2, reflected_ray);

    vec3 reflection = v3_add(v3_multiply(reflection1, 0.5), 
                             v3_multiply(reflection2, 0.5));

    colour = v3_add(v3_multiply(colour, 0.75), v3_multiply(reflection, 0.25));

    return colour;
}

// Returns the shaded color for the given point to shade.
// Calls the relevant shading function based on the material index.
vec3
shade(intersection_point ip)
{
  switch (ip.material)
  {
    case 0:
      return shade_constant(ip);
    case 1:
      return shade_matte(ip);
    case 2:
      return shade_blinn_phong(ip);
    case 3:
      return shade_reflection(ip);
    default:
      return shade_constant(ip);

  }
}

// Determine the surface color for the first object intersected by
// the given ray, or return the scene background color when no
// intersection is found
vec3
ray_color(int level, vec3 ray_origin, vec3 ray_direction)
{
    intersection_point  ip;

    // If this ray has been reflected too many times, simply
    // return the background color.
    if (level >= 3)
        return scene_background_color;

    // Check if the ray intersects anything in the scene
    if (find_first_intersection(&ip, ray_origin, ray_direction))
    {
        // Shade the found intersection point
        ip.ray_level = level;
        return shade(ip);
    }

    // Nothing was hit, return background color
    return scene_background_color;
}
