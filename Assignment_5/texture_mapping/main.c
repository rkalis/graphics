/* Computer Graphics, Assignment "Texture Mapping"
 *
 * Filename ........ main.c
 * Description ..... Creates OpenGL window and draws the scene.
 * Created by ...... Paul Melis, Robert Belleman, Jurgen Sturm
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

#if defined(__GNUC__)
#define glCheckError(s) { GLenum error; if ((error=glGetError())) \
  fprintf(stderr, "* GL error - %s (%s; %s, line %d): %s\n", \
  (s), __FUNCTION__,__FILE__,__LINE__, gluErrorString(error)); }
#else                           /* __GNUC__ */
#define glCheckError(s) { GLenum error; if ((error=glGetError())) \
  fprintf(stderr, "* GL error - %s (line %d): %s\n", \
  (s), __LINE__, gluErrorString(error)); }
#endif                          /* __GNUC__ */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "polys.h"
#include "ppmio.h"
#include "geometry.h"

struct texture_data
{
    const char *filename;
    int         contains_transparency;
};

struct texture_data texture_files[] =
{
    // File name, transparency flag
    { "textures/grass.ppm", 0 },
    { "textures/roof2.ppm", 0 },
    { "textures/wall3.ppm", 0 },
    { "textures/wood.ppm", 0 },
    { "textures/sky.ppm", 0 },
    { "textures/road.ppm", 0 },
    { "textures/banana_leaf2.ppm", 1 },
    { NULL, 0 },
};

GLuint *texture_names;

polys *polylistHouse = NULL;
polys *polylistTreeStem = NULL;
polys *polylistTreeLeafs = NULL;
polys *polylistGround1 = NULL;
polys *polylistGround2 = NULL;
polys *polylistRoad = NULL;
polys *polylistSkydome = NULL;

int window;

int show_textures = 0;
int show_polygons_as_lines = 0;

float camDistance = 24.0;
float camRotZ = -45.0, camAzimuth = 20.0;
float saved_camRotZ, saved_camAzimuth, saved_camDistance;
int mouse_mode = 0;
int mx, my;

GLfloat mat_sun[] = { 1.0, 1.0, 0.5, 1.0 };
GLfloat mat_no_sun[] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat mat_ambient[] = { 0.4, 0.4, 0.4, 1.0 };
GLfloat mat_diffuse[] = { 1, 1, 1, 1.0 };
GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat mat_no_diffuse[] = { 1.0, 0.0, 0.0, 1.0 };
GLfloat mat_no_specular[] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat mat_shininess[] = { 50.0 };
GLfloat mat_no_shininess[] = { 0.0 };

GLfloat light_position[] = { -8.0, 8.0, -8.0, 0.0 };
GLfloat light_ambient[] = { 0.4, 0.4, 0.4, 0.0 };
GLfloat light_diffuse[] = { 0.6, 0.6, 0.6, 0.0 };

// Rough center points of the objects in the scene, used for
// rotating around them with the camera

#define NUM_OBJECTS 3
vec3 object_positions[NUM_OBJECTS] =
{
    /* house, trees, skydome "center" */
    {0.0, 0.0, 7.0},
    {0.0, 0.0, 12.0},
    {0.0, 10.0, 0.0},
};

// Determines around which object the camera rotates
int current_object = 0;

float rand_float(void);

void
InitializePolygonlists(void)
{
    int i;

    // The road
    polylistRoad = CreatePolylist(10);
    loadPolygonalObject(polylistRoad, "road.obj", texture_names, 1.0, 0.0, 0.0, 0.0);

    // We load the ground object twice, but use a different translation to put it once
    // on either side of the road
    polylistGround1 = CreatePolylist(10);
    loadPolygonalObject(polylistGround1, "ground.obj", texture_names, 1.0, 0.0, 0.0, -55.0);
    polylistGround2 = CreatePolylist(10);
    loadPolygonalObject(polylistGround2, "ground.obj", texture_names, 1.0, 0.0, 0.0, 55.0);

    // The skydome
    polylistSkydome = CreatePolylist(1000);
    createHemisphere(polylistSkydome, 100,  0, 0, 0,  1, 1, 1);
    for (i = 0; i < polylistSkydome->length; i++)
        polylistSkydome->items[i].texture_id = texture_names[4];

    // Load the house and translate it, to put it next to the road
    polylistHouse = CreatePolylist(10);
    loadPolygonalObject(polylistHouse, "house.obj", texture_names, 1.0,
        object_positions[0].x, object_positions[0].y, object_positions[0].z);

    // A single tree object
    polylistTreeLeafs = CreatePolylist(10);
    createSphere(polylistTreeLeafs, 0.7, 0.7, 0.7,  0, 1.7, 0,  0, 1, 0);
    for (i = 0; i < polylistTreeLeafs->length; i++)
        polylistTreeLeafs->items[i].texture_id = texture_names[0];

    polylistTreeStem = CreatePolylist(10);
    createCylinder(polylistTreeStem, 0.075, 1.8,  0, 0, 0,  0.5, 0.3, 0);
    for (i = 0; i < polylistTreeStem->length; i++)
        polylistTreeStem->items[i].texture_id = texture_names[3];

    // Done!
    printf("%d polygons\n",
           polylistHouse->length + polylistTreeLeafs->length +
           polylistTreeStem->length + polylistGround1->length + polylistGround2->length);
}


void
InitGL(void)
{
    GLvoid  *image_data;
    int     i, width, height;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (GLfloat) 640 / (GLfloat) 480, 0.1f, 1000.0f);

    glClearColor(0.7, 0.7, 1, 1);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);

    glEnable(GL_COLOR_MATERIAL);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_no_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_no_shininess);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Using GL_REPLACE will effectively disable lighting.
    // The default mode, GL_MODULATE, will use the texture color to
    // module the shaded material color, so the end result still
    // contains the shading.
    //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    // generate texture objects

    for (i = 0; texture_files[i].filename != NULL; i++)
        ;
    texture_names = malloc(i * sizeof(GLuint));
    glGenTextures(i, texture_names);
    glCheckError("glGenTextures");

    // create textures from image files

    GLint   texture_internal_format;
    GLenum  texture_format, texture_type;

    for (i = 0; texture_files[i].filename != NULL; i++)
    {
        image_data = (GLvoid *) readppm(texture_files[i].filename, &width, &height);
        if (image_data)
        {
            printf("texture %d (OpenGL name = %d): %s, %d x %d\n", i,
                texture_names[i], texture_files[i].filename, width, height);

            if (texture_files[i].contains_transparency)
            {
                // Convert the RGB image to RGBA, by replacing
                // red pixels with transparent ones

                // Allocate a temporary RGBA image
                unsigned char *temp_image = (unsigned char*) malloc(width*height*4);
                unsigned char *input = image_data;
                unsigned char *output = temp_image;

                // Convert image to RGBA
                for (int pix = 0; pix < width*height; pix++)
                {
                    if (*input == 255 && *(input+1) == 0 && *(input+2) == 0)
                    {
                        // Input pixel is pure red, make output pixel
                        // white color and fully transparent
                        *output++ = 255;
                        *output++ = 255;
                        *output++ = 255;
                        *output++ = 0;      // alpha
                    }
                    else
                    {
                        // Copy image color, make fully opaque
                        *output++ = *input;
                        *output++ = *(input + 1);
                        *output++ = *(input + 2);
                        *output++ = 255;
                    }

                    input += 3;
                }

                // Replace the RGB image data with the generated RGBA data
                free(image_data);
                image_data = temp_image;

                // This also influences some texture properties
                texture_internal_format = GL_RGBA8;
                texture_format = GL_RGBA;
                texture_type = GL_UNSIGNED_BYTE;
            }
            else
            {
                texture_internal_format = GL_RGB8;
                texture_format = GL_RGB;
                texture_type = GL_UNSIGNED_BYTE;
            }

            glBindTexture(GL_TEXTURE_2D, texture_names[i]);
            glCheckError("glBindTexture");

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 

            /* According to https://www.opengl.org/wiki/Common_Mistakes#gluBuild2DMipmaps
             * you should never use gluBuild2DMipmaps, so instead we're using
             * the advised method
             */
            glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE); 
            glCheckError("glTexParameteri");

            /* Here is the gluBuild2DMipmaps implementation commented out,
             * as we're using the advised method
             */
            // gluBuild2DMipmaps(GL_TEXTURE_2D, texture_internal_format,
            //     width, height, texture_format, texture_type, image_data);
            glTexImage2D(GL_TEXTURE_2D, 0, texture_internal_format,
                width, height, 0, texture_format, texture_type, image_data);
            glCheckError("glTexImage2D");

            // Free the image data, as OpenGL will have made its internal copy by now
            free(image_data);
        }
        else
        {
            perror(texture_files[i].filename);
            exit(0);
        }
    }
}

void
ReSizeGLScene(int Width, int Height)
{
    if (Height == 0)
        Height = 1;

    glViewport(0, 0, Width, Height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (GLfloat) Width / (GLfloat) Height, 0.1f,
                   1000.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void
DrawPolylist(polys * list)
{
    int i, j;

    for (i = 0; i < list->length; i++)
    {
        poly p = list->items[i];

        glColor3f(p.color[0], p.color[1], p.color[2]);

        // Make the correct texture active
        glBindTexture(GL_TEXTURE_2D, p.texture_id);

        glBegin(GL_POLYGON);
        for (j = 0; j < p.points; j++)
        {
            glNormal3f(p.normal[j].x, p.normal[j].y, p.normal[j].z);
            glTexCoord2f(p.tcoord[j].x, p.tcoord[j].y);
            glVertex3f(p.pts[j].x, p.pts[j].y, p.pts[j].z);

        }
        glEnd();
    }
}

void
SetupCamera(void)
{
    float   cx, cy, cz;
    float   t;
    float   beta, gamma;
    vec3    op;

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Verbose, but straightforward way, of positioning the camera.
    // Assume the camera's final position is (cx, cy, cz) and Y is up.
    // Start with c being (camDistance, 0, 0)
    // First rotate around Z, then around Y.
    // Now we have c at the given distance from the origin, with specified rotation angles.

    // degrees -> radians
    beta = camAzimuth / 180.0 * 3.1415926535;
    gamma = camRotZ / 180.0 * 3.1415926535;

    cx = camDistance;
    cy = cz = 0.0;

    // Rotate around Z
    t = cx;
    cx = cx * cos(beta) + cy * sin(beta);
    cy = t * sin(beta) + cy * cos(beta);

    // Rotate around Y
    t = cx;
    cx = cx * cos(gamma) - cz * sin(gamma);
    cz = t * sin(gamma) + cz * cos(gamma);

    gluLookAt(cx, cy, cz, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    // Translate view to rotate around the selected object
    op = object_positions[current_object];
    glTranslatef(-op.x, -op.y, -op.z);
}

void
DrawGLScene(void)
{
    float   tx, tz;

    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    // set camera based on previous mouse input
    SetupCamera();

    // Set the light position. We do that after setting up the
    // camera so that the light position will keep its world position
    // and rotate along with the scene.
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    // Draw the different objects

    if (show_polygons_as_lines)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDisable(GL_TEXTURE_2D);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        if (show_textures)
        {
            glEnable(GL_ALPHA_TEST);
            glAlphaFunc(GL_GREATER, 0.5);
            glEnable(GL_TEXTURE_2D);
        }
        else
            glDisable(GL_TEXTURE_2D);
    }

    DrawPolylist(polylistHouse);
    DrawPolylist(polylistGround1);
    DrawPolylist(polylistGround2);
    DrawPolylist(polylistRoad);

    // Draw a number of trees around the house.
    // They are all identical, but differ in location and Y-rotation.

    // Re-seed the random generator, so we always get the same sequence
    // back from rand_float() below, for different runs of the program.
    srand(95497452);

    for (int t = 0; t < 12; t++)
    {
        glPushMatrix();

        tx = 10 * (rand_float()-0.5) + object_positions[0].x;
        tz = 3 * rand_float() + 2.0 + object_positions[0].z;
        glTranslatef(tx, 0, tz);

        glRotatef(rand_float()*360.0, 0, 1, 0);
        glScalef(1, 1 + (rand_float()-0.5)*0.6, 1);

        DrawPolylist(polylistTreeStem);
        DrawPolylist(polylistTreeLeafs);

        glPopMatrix();
    }

    // Draw the skydome with lighting turned off

    glPushAttrib(GL_LIGHTING_BIT);
    glDisable(GL_LIGHTING);
    DrawPolylist(polylistSkydome);
    glPopAttrib();

    glutSwapBuffers();
}

// Return a (pseudo-)random floating-point value in the range [0,1]
float
rand_float(void)
{
    return (float)(1.0 * rand() / RAND_MAX);
}

void
keyPressed(unsigned char key, int x, int y)
{
    (void)x;
    (void)y;

    switch (key)
    {
        case 'o':
            current_object = (current_object + 1) % NUM_OBJECTS;
            glutPostRedisplay();
            break;
        case 't':
            show_textures = 1 - show_textures;
            glutPostRedisplay();
            break;
        case 'l':
            show_polygons_as_lines = 1;
            glutPostRedisplay();
            break;
        case 'p':
            show_polygons_as_lines = 0;
            glutPostRedisplay();
            break;
        case 'q':
            exit(-1);
    }
}

void
specialKeyPressed(int key, int x, int y)
{
    (void)x;
    (void)y;
    (void)key;

    DrawGLScene();
}

static void
mouseFunc(int button, int state, int x, int y)
{
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
        } else if (button == GLUT_RIGHT_BUTTON)
        {
            mouse_mode = GLUT_RIGHT_BUTTON;

            saved_camDistance = camDistance;

            my = y;
        }
    } else if (state == GLUT_UP && button == mouse_mode)
    {
        // pressed button released
        mouse_mode = 0;
    }

}

static void
motionFunc(int x, int y)
{
    int dx, dy;

    if (mouse_mode == GLUT_LEFT_BUTTON)
    {
        dx = mx - x;
        dy = my - y;

        camRotZ = saved_camRotZ - dx * 0.25;
        camAzimuth = saved_camAzimuth - dy * 0.25;

        if (camAzimuth > 89.99)
            camAzimuth = 89.99;
        else if (camAzimuth < -89.99)
            camAzimuth = -89.99;
    } else if (mouse_mode == GLUT_RIGHT_BUTTON)
    {
        dy = my - y;

        camDistance = saved_camDistance - dy * 0.5;
        if (camDistance < 0.1)
            camDistance = 0.1;
        else if (camDistance > 1000.0)
            camDistance = 1000.0;
    }
}

int
main(int argc, char **argv)
{
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_ALPHA | GLUT_DEPTH);
    glutInitWindowSize(640, 480);
    glutInitWindowPosition(0, 0);
    window = glutCreateWindow("OpenGL Framework");

    glutDisplayFunc(&DrawGLScene);
    glutIdleFunc(&DrawGLScene);
    glutReshapeFunc(&ReSizeGLScene);
    glutSpecialFunc(&specialKeyPressed);
    glutKeyboardFunc(&keyPressed);
    glutMouseFunc(&mouseFunc);
    glutMotionFunc(&motionFunc);

    InitGL();
    InitializePolygonlists();

    glutMainLoop();

    return 1;
}
