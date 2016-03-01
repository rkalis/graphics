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

#ifdef OS_X
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "plymodel.h"
#include "v3math.h"
#include "shaders.h"
#include "perlin.h"
#include "intersection.h"
#include "scene.h"
#include "constants.h"
#include "bvh.h"

// Number of drawable pixels, i.e. x coordinates passed to PutPixel()
// should be in the range [0, framebuffer_width[.  Analogous for y.
// (These values must both be a power of 2)
const int   framebuffer_width = 512;
const int   framebuffer_height = 512;

// Camera vertical field-of-view
const float VFOV = 45.0;

byte    *framebuffer;

int     show_raytraced=0;
int     needs_rerender=1;
int     show_bvh=0;
int     draw_bvh_mode=0;
int     show_normals=0;
int     do_antialiasing=0;

float   camDistance = 6.5;
float   camRotZ = 25.0, camAzimuth = -40.0;
float   saved_camRotZ, saved_camAzimuth, saved_camDistance;
int     mouse_mode = 0;
int     mx, my;

viewpoint   viewpoints[6] =
{
    { -40.0, 25.0, 6.5 },
    { -27.5, -30.5, 2.0 },
    { -73.8, 37.0, 3.8 },
    {  46.2, 0.0, 6.2 },
    { -35.0, -187.2, 2.8 },
    { -80.3, 27.0, 1.1 },
};

void
init_opengl(void)
{
    // Set the background color
    glClearColor(1, 1, 1, 0);

    // Allocate a framebuffer, to be filled during ray tracing
    framebuffer = (byte*) malloc(framebuffer_width*framebuffer_height*3);

    // Setup texturing state (as we display the framebuffer
    // using a textured quad)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    GLfloat light_ambient[] = {0.4,0.4,0.4,0.0};
    GLfloat mat_shininess[] = { 50.0 };
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);
}

void
resize(int w, int h)
{
    if (h == 0)
        h = 1;

    glViewport(0, 0, w, h);
}

void
put_pixel(int x, int y, float r, float g, float b)
{
    if (x < 0 || y < 0 || x >= framebuffer_width || y >= framebuffer_height)
    {
        printf("put_pixel(): x, y coordinates (%d, %d) outside of visible area!\n",
                x, y);
        return;
    }

    // The pixels in framebuffer[] are layed out sequentially,
    // with the R, G and B values one after the each, e.g
    // RGBRGBRGB...

    framebuffer[3*(framebuffer_width*y+x)] = (int)(255*r);
    framebuffer[3*(framebuffer_width*y+x)+1] = (int)(255*g);
    framebuffer[3*(framebuffer_width*y+x)+2] = (int)(255*b);
}

void
setup_camera(void)
{
    float	cx, cy, cz;
    float	t;
    float 	beta, gamma;

    // degrees -> radians
    beta = camAzimuth / 180.0 * M_PI;
    gamma = camRotZ / 180.0 * M_PI;

    cx = camDistance;
    cy = cz = 0.0;

    // Rotate around Y
    t = cx;
    cx = cx * cos(beta) + cz * sin(beta);
	// cy remains unchanged
    cz = -t * sin(beta) + cz * cos(beta);

    // Rotate around Z
    t = cx;
    cx = cx * cos(gamma) - cy * sin(gamma);
    cy = t * sin(gamma) + cy * cos(gamma);
	// cz remains unchanged

    scene_camera_position.x = cx;
    scene_camera_position.y = cy;
    scene_camera_position.z = cz;

    scene_camera_lookat.x = 0.0;
    scene_camera_lookat.y = 0.0;
    scene_camera_lookat.z = 0.5;

    // Assumes matrix mode is model-view
    glLoadIdentity();
    gluLookAt (cx, cy, cz,  0.0, 0.0, 0.5,  0.0, 0.0, 1.0);
}

void
ray_trace(void)
{
    vec3    forward_vector, right_vector, up_vector;
    int     i, j;
    float   image_plane_width, image_plane_height;
    vec3    color;
    char    buf[128];

    struct timeval  t0, t1;
    float           time_taken;

    fprintf(stderr, "Ray tracing ...");
    gettimeofday(&t0, NULL);

    num_rays_shot = num_shadow_rays_shot = num_triangles_tested = num_bboxes_tested = 0;

    // Compute camera coordinate system from camera position
    // and look-at point
    up_vector = v3_create(0, 0, 1);
    forward_vector = v3_normalize(v3_subtract(scene_camera_lookat, scene_camera_position));
    right_vector = v3_normalize(v3_crossprod(forward_vector, up_vector));
    up_vector = v3_crossprod(right_vector, forward_vector);

    // Compute size of image plane from the chosen field-of-view
    // and image aspect ratio. This is the size of the plane at distance
    // of one unit from the camera position.
    image_plane_height = 2.0 * tan(0.5*VFOV/180*M_PI);
    image_plane_width = image_plane_height * (1.0 * framebuffer_width / framebuffer_height);

    float middle_x = image_plane_width / 2;
    float middle_y = image_plane_height / 2;


    // Loop over all pixels in the framebuffer
    for (j = 0; j < framebuffer_height; j++)
    {
        for (i = 0; i < framebuffer_width; i++)
        {
            float img_i = i * 1.0 / framebuffer_width * image_plane_width;
            float img_j = j * 1.0 / framebuffer_height * image_plane_height;

            float right_factor = (img_i - middle_x);
            float up_factor = (middle_y - img_j);

            vec3 right_component = v3_multiply(right_vector, right_factor);
            vec3 up_component = v3_multiply(up_vector, up_factor);
            vec3 ray = v3_add(v3_add(right_component, up_component), forward_vector);

            color = ray_color(0, scene_camera_position, ray);
            // Output pixel color
            put_pixel(i, j, color.x, color.y, color.z);
        }

        sprintf(buf, "Ray-tracing ::: %.0f%% done", 100.0*j/framebuffer_height);
        glutSetWindowTitle(buf);
    }
    color = ray_color(0, scene_camera_position, forward_vector);
    // Output pixel color
    put_pixel(0, 0, color.x, color.y, color.z);

    // Done!
    gettimeofday(&t1, NULL);

    glutSetWindowTitle("Ray-tracing ::: done");

    // Output some statistics
    time_taken = 1.0 * (t1.tv_sec - t0.tv_sec) + (t1.tv_usec - t0.tv_usec) / 1000000.0;

    fprintf(stderr, " done in %.1f seconds\n", time_taken);
    fprintf(stderr, "... %lld total rays shot, of which %d camera rays and "
            "%lld shadow rays\n", num_rays_shot,
            do_antialiasing ? 4*framebuffer_width*framebuffer_height :
                              framebuffer_width*framebuffer_height,
            num_shadow_rays_shot);
    fprintf(stderr, "... %lld triangles intersection tested "
            "(avg %.1f tri/ray)\n",
        num_triangles_tested, 1.0*num_triangles_tested/num_rays_shot);
    fprintf(stderr, "... %lld bboxes intersection tested (avg %.1f bbox/ray)\n",
        num_bboxes_tested, 1.0*num_bboxes_tested/num_rays_shot);
}

// Draw the node bboxes of the BVH, for inner nodes at a certain
// level in the tree

static void
draw_bvh_inner_nodes(int level, bvh_node* node)
{
    vec3    center, size;

    if (node->is_leaf)
        return;

    if (level == draw_bvh_mode)
    {
        center = v3_multiply(v3_add(node->bbox.min, node->bbox.max), 0.5);
        size = v3_subtract(node->bbox.max, node->bbox.min);

        glColor3f(1, 0, 0);
        glPushMatrix();
        glTranslatef(center.x, center.y, center.z);
        glScalef(size.x, size.y, size.z);
        glutWireCube(1.0);
        glPopMatrix();
    }
    else
    {
        draw_bvh_inner_nodes(level+1, node->u.inner.left_child);
        draw_bvh_inner_nodes(level+1, node->u.inner.right_child);
    }
}

// Draw leaf node bounding boxes

static void
draw_bvh_leaf_nodes(bvh_node* node)
{
    vec3    center, size;

    if (node->is_leaf)
    {
        center = v3_multiply(v3_add(node->bbox.min, node->bbox.max), 0.5);
        size = v3_subtract(node->bbox.max, node->bbox.min);

        glColor3f(0, 0, 1);
        glPushMatrix();
        glTranslatef(center.x, center.y, center.z);
        glScalef(size.x, size.y, size.z);
        glutWireCube(1.0);
        glPopMatrix();
    }
    else
    {
        draw_bvh_leaf_nodes(node->u.inner.left_child);
        draw_bvh_leaf_nodes(node->u.inner.right_child);
    }
}

void
draw_scene(void)
{
    // clear the draw buffer
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    if (show_raytraced)
    {
        if (needs_rerender)
        {
            // clear the framebuffer
            memset(framebuffer, 255, 3*framebuffer_width*framebuffer_height);

            // trace a new picture
            ray_trace();

            needs_rerender = 0;
        }

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        // Draw the framebuffer using a textured quad

        glOrtho(0, framebuffer_width, 0, framebuffer_height, -1, 1);

        glDisable(GL_LIGHTING);
        glDisable(GL_CULL_FACE);

        glEnable(GL_TEXTURE_2D);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8,
            framebuffer_width, framebuffer_height,
            0, GL_RGB, GL_UNSIGNED_BYTE, framebuffer);

        glColor3f(1, 1, 1);
        glBegin(GL_QUADS);
            glTexCoord2i(0, 0);
            glVertex2i(0, framebuffer_height);

            glTexCoord2i(0, 1);
            glVertex2i(0, 0);

            glTexCoord2i(1, 1);
            glVertex2i(framebuffer_width, 0);

            glTexCoord2i(1, 0);
            glVertex2i(framebuffer_width, framebuffer_height);
        glEnd();
    }
    else
    {
        // Draw scene using OpenGL

        //glutSetWindowTitle("OpenGL view");

        glMatrixMode(GL_PROJECTION);

        glLoadIdentity();
        gluPerspective(VFOV, 1.0*framebuffer_width/framebuffer_height, 0.1, 1000.0);

        glMatrixMode(GL_MODELVIEW);

        setup_camera();

        glEnable(GL_LIGHTING);
        glEnable(GL_CULL_FACE);
        glDisable(GL_TEXTURE_2D);

        // Try to set up the lighting to match the scene

        GLfloat v[4];

        v[0] = scene_ambient_light;
        v[1] = scene_ambient_light;
        v[2] = scene_ambient_light;
        v[3] = 1.0;
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, v);

        for (int l = 0; l < scene_num_lights; l++)
        {
            glEnable(GL_LIGHT0 + l);

            v[0] = scene_lights[l].position.x;
            v[1] = scene_lights[l].position.y;
            v[2] = scene_lights[l].position.z;
            v[3] = 1.0; // we want a positional light source
            glLightfv(GL_LIGHT0 + l, GL_POSITION, v);

            v[0] = v[1] = v[2] = v[3] = 0.0;
            glLightfv(GL_LIGHT0 + l, GL_AMBIENT, v);
            glLightfv(GL_LIGHT0 + l, GL_SPECULAR, v);

            v[0] = v[1] = v[2] = scene_lights[l].intensity;
            v[3] = 1.0;
            glLightfv(GL_LIGHT0 + l, GL_DIFFUSE, v);
        }

        GLfloat one[] = { 1.0, 1.0, 1.0, 1.0 };
        GLfloat zero[] = { 0.0, 0.0, 0.0, 1.0 };

        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, zero);
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, zero);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, one);

		// Draw the triangles in the scene

        triangle    tri;
        int         p, q, r;

        glBegin(GL_TRIANGLES);

        for (int t = 0; t < scene_num_triangles; t++)
        {
            tri = scene_triangles[t];
            p = tri.v[0];
            q = tri.v[1];
            r = tri.v[2];

            glNormal3f(tri.vn[0].x, tri.vn[0].y, tri.vn[0].z);
            glVertex3f(scene_vertices[p].x, scene_vertices[p].y, scene_vertices[p].z);
            glNormal3f(tri.vn[1].x, tri.vn[1].y, tri.vn[1].z);
            glVertex3f(scene_vertices[q].x, scene_vertices[q].y, scene_vertices[q].z);
            glNormal3f(tri.vn[2].x, tri.vn[2].y, tri.vn[2].z);
            glVertex3f(scene_vertices[r].x, scene_vertices[r].y, scene_vertices[r].z);
        }

        glEnd();

        if (show_normals)
        {
            // Draw vertex normals as red lines
            glDisable(GL_LIGHTING);
            glColor3f(1, 0, 0);
            glBegin(GL_LINES);

            vec3    n;
            for (int t = 0; t < scene_num_triangles; t++)
            {
                tri = scene_triangles[t];

                for (int vi = 0; vi < 3; vi++)
                {
                    p = tri.v[vi];
                    n = scene_vertices[p];
                    glVertex3f(n.x, n.y, n.z);
                    n = v3_add(n, v3_multiply(tri.vn[vi], 0.05));
                    glVertex3f(n.x, n.y, n.z);
                }
            }

            glEnd();

            glEnable(GL_LIGHTING);
        }

		// Draw the spheres in the scene

        for (int s = 0; s < scene_num_spheres; s++)
        {
            glPushMatrix();
            glTranslatef(scene_spheres[s].center.x, scene_spheres[s].center.y, scene_spheres[s].center.z);
            glutSolidSphere(scene_spheres[s].radius, 12, 12);
            glPopMatrix();
        }

        // Show BVH node bboxes

        if (show_bvh)
        {
            glDisable(GL_LIGHTING);
            if (draw_bvh_mode == 0)
                draw_bvh_leaf_nodes(bvh_root);
            else
                draw_bvh_inner_nodes(1, bvh_root);
        }

		/*
		// Draw some axes

		glDisable(GL_LIGHTING);

		glBegin(GL_LINES);
			glColor3f(1, 0, 0);
			glVertex3f(0, 0, 0);
			glVertex3f(10, 0, 0);
			glColor3f(0, 1, 0);
			glVertex3f(0, 0, 0);
			glVertex3f(0, 10, 0);
			glColor3f(0, 0, 1);
			glVertex3f(0, 0, 0);
			glVertex3f(0, 0, 10);
		glEnd();
		*/
    }

    // finally, swap the draw buffers to make the triangles appear on screen
    glutSwapBuffers();
}

void
save_image(void)
{
    FILE *f;

    f = fopen("image.ppm", "wt");
    if (!f)
    {
        fprintf(stderr, "Could not create image file\n");
        return;
    }

    fprintf(f, "P3\n# Raytraced image\n%d %d\n255\n", framebuffer_width, framebuffer_height);
    for (int i = 0; i < 3*framebuffer_width*framebuffer_height; i++)
        fprintf(f, "%d\n", framebuffer[i]);

    printf("Image saved to image.ppm\n");

    fclose(f);
}

void
key_pressed(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 'r':
        {
            // Toggle between OpenGL and ray-traced output
            show_raytraced = 1 - show_raytraced;
            if (show_raytraced)
                glutSetWindowTitle("Ray-tracing [ray-traced output]");
            else
                glutSetWindowTitle("Ray-tracing [OpenGL view]");
            glutPostRedisplay();
            break;
        }
        case 'a':
        {
            // Toggle anti-aliasing (forces immediate re-render)
            do_antialiasing = 1 - do_antialiasing;
            needs_rerender = 1;
            glutPostRedisplay();
            break;
        }
        case 'b':
        {
            // Toggle use of the BVH for intersection testing
            // (forces immediate re-render)
            use_bvh = 1 - use_bvh;
            printf("use_bvh set to %d\n", use_bvh);
            needs_rerender = 1;
            glutPostRedisplay();
            break;
        }
        case 'B':
        {
            // Show BVH nodes
            show_bvh = 1 - show_bvh;
            if (show_bvh)
                draw_bvh_mode = 0;
            glutPostRedisplay();
            break;
        }
        case ']':
        {
            draw_bvh_mode++;
            glutPostRedisplay();
            break;
        }
        case '[':
        {
            draw_bvh_mode--;
            if (draw_bvh_mode < 0)
                draw_bvh_mode = 0;
            glutPostRedisplay();
            break;
        }
        case 'n':
        {
            // Show vertex normals
            show_normals = 1 - show_normals;
            glutPostRedisplay();
            break;
        }
        case 'c':
        {
            // Dump camera parameters
            printf("azimuth = %.1f, rot_z = %.1f, distance = %.1f\n",
                camAzimuth, camRotZ, camDistance);
            break;

        }
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        {
            // Switch to a predefined viewpoint
            int idx = (int)(key - '1');
            camAzimuth = viewpoints[idx].azimuth;
            camRotZ = viewpoints[idx].rot_z;
            camDistance = viewpoints[idx].distance;

            // Switch to OpenGL viewing
            show_raytraced = 0;

            // And since the camera params changed we need to rerender
            needs_rerender = 1;

            glutPostRedisplay();

            break;
        }
        case 's':
        {
            // Save rendered image to (ascii) .ppm file 'image.ppm'
            save_image();
            break;
        }
        case 'm':
        {
            // Display mouse coordinates (for debugging only)
            printf("x = %d, y = %d\n", x, y);
            break;
        }
        case 'q':
            exit(0);
    }
}

static void
mouse_func(int button, int state, int x, int y)
{
    if (show_raytraced)
        return;

    // guard against both left and right buttons being pressed at the same time,
    // by only responding when a mouse button is pressed while another one
    // hasn't been pressed yet
    if (state == GLUT_DOWN && mouse_mode == 0)
    {
        if (button == GLUT_LEFT_BUTTON)
        {
            mouse_mode = GLUT_LEFT_BUTTON;

            saved_camRotZ = camRotZ;
            saved_camAzimuth = camAzimuth;

            mx = x;
            my = y;
        }
        else if (button == GLUT_RIGHT_BUTTON)
        {
            mouse_mode = GLUT_RIGHT_BUTTON;

            saved_camDistance = camDistance;

            my = y;
        }
    }
    else if (state == GLUT_UP && button == mouse_mode)
    {
        // pressed button released
        mouse_mode = 0;
    }

}

static void
motion_func(int x, int y)
{
    int dx, dy;

    if (show_raytraced)
        return;

    if (mouse_mode == GLUT_LEFT_BUTTON)
    {
        dx = mx - x;
        dy = my - y;

        camRotZ = saved_camRotZ + dx * 0.25;
        camAzimuth = saved_camAzimuth + dy * 0.25;

        if (camAzimuth > 89.99)
            camAzimuth = 89.99;
        else if (camAzimuth < -89.99)
            camAzimuth = -89.99;

        needs_rerender = 1;
    }
    else if (mouse_mode == GLUT_RIGHT_BUTTON)
    {
        dy = my - y;

        camDistance = saved_camDistance - dy * 0.15;
        if (camDistance < 0.5)
            camDistance = 0.5;
        else if (camDistance > 100.0)
            camDistance = 100.0;

        needs_rerender = 1;
    }
}


int
main(int argc, char **argv)
{
    glutInit(&argc, argv);

    if (--argc != 1)
    {
        printf("Usage: %s file.scn\n", argv[0]);
        exit(-1);
    }

    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    glutInitWindowSize(framebuffer_width, framebuffer_height);

    glutInitWindowPosition(20, 100);
    glutCreateWindow("Ray tracing");

    glutDisplayFunc(&draw_scene);
    glutIdleFunc(&draw_scene);
    glutReshapeFunc(&resize);
    //glutSpecialFunc(&specialKeyPressed);
    glutKeyboardFunc(&key_pressed);
    glutMouseFunc(&mouse_func);
    glutMotionFunc(&motion_func);

	read_scene(argv[1]);

    init_opengl();
    init_noise();

    glutMainLoop();

    return 1;
}
