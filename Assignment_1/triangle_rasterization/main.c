/* Computer Graphics assignment, Triangle Rasterization
 *
 * Created by ...... Paul Melis
 */

#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "types.h"
#include "trirast.h"

#include "triangles.h"

// Number of drawable pixels, i.e. x coordinates passed to PutPixel()
// should be in the range [0, framebuffer_width[.  Analogous for y.
// (These values must both be a power of 2)
const int   framebuffer_width = 128;
const int   framebuffer_height = 64;

const int   zoomed_pixel_size = 7;

int     screen_width, screen_height;
int     draw_optimized = 0;
int     zoom = 1;
int     scene = 1;
int     draw_corners = 0;
int     color_by_putpixel_count = 0;

byte    *framebuffer;

void
InitOpenGL(void)
{
    // Set the background color
    glClearColor(0., 0., 0., 0.);

    // Allocate a framebuffer, to be filled during triangle rasterization
    framebuffer = (byte*)malloc(framebuffer_width*framebuffer_height*3);

    // Setup texturing state (as we display the rasterization framebuffer
    // using a textured quad)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);
}

void PutPixel(int x, int y, byte r, byte g, byte b)
{
    if (x < 0 || y < 0 || x >= framebuffer_width || y >= framebuffer_height)
    {
        printf("PutPixel(): x, y coordinates (%d, %d) outside of visible area!\n",
                x, y);
        return;
    }

    // The pixels in framebuffer[] are layed out sequentially,
    // with the R, G and B values one after the other, e.g
    // RGBRGBRGB...
    framebuffer[3*(framebuffer_width*y+x)] = r;
    framebuffer[3*(framebuffer_width*y+x)+1] = g;
    framebuffer[3*(framebuffer_width*y+x)+2] = b;
}

void
DrawTriangles(void)
{
    struct  triangle tri;
    for (unsigned int t = 0; t < sizeof(triangles)/sizeof(struct triangle); t++)
    {
        tri = triangles[t];

        if (draw_optimized)
        {
            /* draw the triangle with the given color */
            draw_triangle_optimized(
                vertices[tri.i].x, vertices[tri.i].y,
                vertices[tri.j].x, vertices[tri.j].y,
                vertices[tri.k].x, vertices[tri.k].y,
                colors[tri.c].r, colors[tri.c].g, colors[tri.c].b);

        }
        else
        {
            /* draw the triangle with the given color */
            draw_triangle(
                vertices[tri.i].x, vertices[tri.i].y,
                vertices[tri.j].x, vertices[tri.j].y,
                vertices[tri.k].x, vertices[tri.k].y,
                colors[tri.c].r, colors[tri.c].g, colors[tri.c].b);
        }

        if (draw_corners)
        {
            PutPixel(vertices[tri.i].x, vertices[tri.i].y, 255, 255, 255);
            PutPixel(vertices[tri.j].x, vertices[tri.j].y, 255, 255, 255);
            PutPixel(vertices[tri.k].x, vertices[tri.k].y, 255, 255, 255);
        }
    }
}

void
DrawTrianglesOpenGL(void)
{
    struct  triangle tri;

    glDisable(GL_TEXTURE_2D);

    glBegin(GL_TRIANGLES);
    for (unsigned int t = 0; t < sizeof(triangles)/sizeof(struct triangle); t++)
    {
        tri = triangles[t];

        /* draw the triangle with the given color */
        glColor3ub(colors[tri.c].r, colors[tri.c].g, colors[tri.c].b);
        glVertex2f(vertices[tri.i].x, vertices[tri.i].y);
        glVertex2f(vertices[tri.j].x, vertices[tri.j].y);
        glVertex2f(vertices[tri.k].x, vertices[tri.k].y);
    }
    glEnd();

    if (draw_corners)
    {
        glColor3ub(255, 255, 255);
        glBegin(GL_POINTS);
        for (unsigned int t = 0; t < sizeof(triangles)/sizeof(struct triangle); t++)
        {
            tri = triangles[t];
            glVertex2f(vertices[tri.i].x, vertices[tri.i].y);
            glVertex2f(vertices[tri.j].x, vertices[tri.j].y);
            glVertex2f(vertices[tri.k].x, vertices[tri.k].y);
        }
        glEnd();
    }
}

void
TestRasterizationSpeed(void)
{
    const int N = 1000;

    struct timeval  t0, t1;
    double          diff;

    //srand(123456);

    gettimeofday(&t0, NULL);

    if (draw_optimized)
    {
        for (int t = 0; t < N; t++)
        {
            draw_triangle_optimized(
                rand()%framebuffer_width, rand()%framebuffer_height,
                rand()%framebuffer_width, rand()%framebuffer_height,
                rand()%framebuffer_width, rand()%framebuffer_height,
                colors[t%6].r, colors[t%6].g, colors[t%6].b);
        }
    }
    else
    {
        for (int t = 0; t < N; t++)
        {
            draw_triangle(
                rand()%framebuffer_width, rand()%framebuffer_height,
                rand()%framebuffer_width, rand()%framebuffer_height,
                rand()%framebuffer_width, rand()%framebuffer_height,
                colors[t%6].r, colors[t%6].g, colors[t%6].b);
        }
    }

    gettimeofday(&t1, NULL);

    /* calculate time used */
    diff = t1.tv_sec - t0.tv_sec + (t1.tv_usec - t0.tv_usec)*1.0e-6;

    printf("%d triangles in %.6f seconds, %.1f triangles/sec\n", N, diff, N/diff);
}

void
DrawPixels(void)
{
    const int N = 1000000;

    struct timeval  t0, t1;

    gettimeofday(&t0, NULL);

    srand(123456);
    for (int i = 0; i < N; i++)
    {
        PutPixel(rand()%framebuffer_width, rand()%framebuffer_height,
            rand()%255, rand()%255, rand()%255);
    }

    gettimeofday(&t1, NULL);

    /* calculate time used */
    double diff = (t1.tv_sec + t1.tv_usec/1000000.0) - (t0.tv_sec + t0.tv_usec/1000000.0);

    printf("%d pixels in %.6f seconds, %.1f pixels/sec\n", N, diff, N/diff);
}

void
DrawScene(void)
{

    /* clear the draw buffer */
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    // clear the rasterization framebuffer
    memset(framebuffer, 0, 3*framebuffer_width*framebuffer_height);

    if (scene == 1)
        DrawTriangles();
    else if (scene == 2)
        TestRasterizationSpeed();
    else if (scene == 4)
        DrawPixels();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (scene != 9)
    {
        if (zoom)
            glOrtho(0, framebuffer_width, 0, framebuffer_height, -1, 1);
        else
            glOrtho(0, screen_width, 0, screen_height, -1, 1);

        // Draw textured quad

        glEnable(GL_TEXTURE_2D);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8,
            framebuffer_width, framebuffer_height,
            0, GL_RGB, GL_UNSIGNED_BYTE, framebuffer);

        glColor3f(1, 1, 1);
        glBegin(GL_QUADS);
            glTexCoord2i(0, 0);
            glVertex2i(0, 0);
            glTexCoord2i(1, 0);
            glVertex2i(framebuffer_width, 0);
            glTexCoord2i(1, 1);
            glVertex2i(framebuffer_width, framebuffer_height);
            glTexCoord2i(0, 1);
            glVertex2i(0, framebuffer_height);
        glEnd();
    }
    else
    {
        if (zoom)
            glOrtho(-0.5, framebuffer_width-0.5, -0.5, framebuffer_height-0.5, -1, 1);
        else
            glOrtho(-0.5, screen_width-0.5, -0.5, screen_height-0.5, -1, 1);

        DrawTrianglesOpenGL();

        glDisable(GL_TEXTURE_2D);
        glColor3f(1, 1, 0);
        glBegin(GL_POINTS);
            glVertex2i(0, 0);
            glVertex2i(framebuffer_width-1, framebuffer_height-1);
        glEnd();

    }

    // finally, swap the draw buffers to make the triangles appear on screen
    glutSwapBuffers();
}

void
KeyPressed(unsigned char key, int x, int y)
{
    switch (key)
    {
        case '1':
        case '2':
        case '3':
        case '4':
        case '9':
        {
            scene = key - '0';
            glutPostRedisplay();
            break;
        }
        case 'o':
        {
            draw_optimized = 1 - draw_optimized;
            printf("draw_optimized set to %d\n", draw_optimized);
            glutPostRedisplay();
            break;
        }
        case 'z':
        {
            zoom = 1 - zoom;
            glutPostRedisplay();
            break;
        }
        case 'd':
        {
            // debug
            color_by_putpixel_count = 1 - color_by_putpixel_count;
            printf("color_by_putpixel_count set to %d\n", color_by_putpixel_count);
            glutPostRedisplay();
            break;
        }
        case 'c':
        {
            // triangle corners
            draw_corners = 1 - draw_corners;
            glutPostRedisplay();
            break;
        }
        case 'q':
            exit(0);
    }
}

int
main(int argc, char **argv)
{
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    screen_width = framebuffer_width * zoomed_pixel_size;
    screen_height = framebuffer_height * zoomed_pixel_size;
    glutInitWindowSize(screen_width, screen_height);

    glutInitWindowPosition(20, 20);
    glutCreateWindow("Triangle rasterization");

    glutDisplayFunc(&DrawScene);
    glutIdleFunc(&DrawScene);
    //glutReshapeFunc(&ReSizeScene);
    //glutSpecialFunc(&specialKeyPressed);
    glutKeyboardFunc(&KeyPressed);
    //glutMouseFunc(&mouseFunc);
    //glutMotionFunc(&motionFunc);

    InitOpenGL();

    glutMainLoop();

    return 1;
}
