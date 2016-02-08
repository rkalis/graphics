/* Computer Graphics, Assignment 5, Orthogonal projection
 *
 * Filename ........ main.c
 * Description ..... Creates OpenGL window and draws the scene.
 * Date ............ 01.09.2006
 * Created by ...... Jurgen Sturm 
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>   
#include <GL/gl.h>
#include <GL/glu.h>
#include "ortho.h"
#include <math.h>

int window;
int useMyOrtho=0;

int frame=0;
int doRotate=1;
int doTranslate=1;
int doScale=1;
double cameraRotation=0.2;
double cameraHeight=100;
double cameraRotationSpeed=0.007;
double cameraHeightTarget=3;
int windowWidth=0,windowHeight=0;
double speed=0.01;

void InitGL(int Width, int Height)	  
{
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClearDepth(1.0);
  glDepthFunc(GL_LESS); 
  glEnable(GL_DEPTH_TEST);      
  glShadeModel(GL_SMOOTH);
}

void ReSizeGLScene(int Width, int Height)
{
  int xRange = 10, yRange = 10, zRange = 100;
  windowWidth=Width;
  windowHeight=Height;
  if (Height==0)
    Height=1;

  glViewport(0, 0, Width, Height);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  if(useMyOrtho) {
    if (Width <= Height)
      myOrtho (-xRange, xRange, -yRange*Height/Width, yRange*Height/Width, -zRange, zRange);
    else
      myOrtho (-xRange*Width/Height, xRange*Width/Height, -yRange, yRange, -zRange, zRange);  
  } else {
    if (Width <= Height)
      glOrtho (-xRange, xRange, -yRange*Height/Width, yRange*Height/Width, -zRange, zRange);
    else
      glOrtho (-xRange*Width/Height, xRange*Width/Height, -yRange, yRange, -zRange, zRange);  
  }
  /*
  glLoadIdentity();
  gluPerspective(45.0f,(GLfloat)Width/(GLfloat)Height,0.1f,100.0f);
  */
  glMatrixMode(GL_MODELVIEW);
}

void drawCube() {
  glBegin(GL_QUADS);				/* start drawing the cube. */
  
  /* top of cube */
  glColor3f(0.0f,1.0f,0.0f);
  glVertex3f( 1.0f, 1.0f,-1.0f);
  glVertex3f(-1.0f, 1.0f,-1.0f);		
  glVertex3f(-1.0f, 1.0f, 1.0f);		
  glVertex3f( 1.0f, 1.0f, 1.0f);		

  /* bottom of cube */
  glColor3f(1.0f,0.5f,0.0f);			
  glVertex3f( 1.0f,-1.0f, 1.0f);		
  glVertex3f(-1.0f,-1.0f, 1.0f);		
  glVertex3f(-1.0f,-1.0f,-1.0f);		
  glVertex3f( 1.0f,-1.0f,-1.0f);		

  /* front of cube */
  glColor3f(1.0f,0.0f,0.0f);			
  glVertex3f( 1.0f, 1.0f, 1.0f);		
  glVertex3f(-1.0f, 1.0f, 1.0f);		
  glVertex3f(-1.0f,-1.0f, 1.0f);		
  glVertex3f( 1.0f,-1.0f, 1.0f);		

  /* back of cube. */
  glColor3f(1.0f,1.0f,0.0f);			
  glVertex3f( 1.0f,-1.0f,-1.0f);		
  glVertex3f(-1.0f,-1.0f,-1.0f);		
  glVertex3f(-1.0f, 1.0f,-1.0f);		
  glVertex3f( 1.0f, 1.0f,-1.0f);		

  /* left of cube */
  glColor3f(0.0f,0.0f,1.0f);			
  glVertex3f(-1.0f, 1.0f, 1.0f);		
  glVertex3f(-1.0f, 1.0f,-1.0f);		
  glVertex3f(-1.0f,-1.0f,-1.0f);		
  glVertex3f(-1.0f,-1.0f, 1.0f);		

  /* Right of cube */
  glColor3f(1.0f,0.0f,1.0f);			
  glVertex3f( 1.0f, 1.0f,-1.0f);	        
  glVertex3f( 1.0f, 1.0f, 1.0f);		
  glVertex3f( 1.0f,-1.0f, 1.0f);		
  glVertex3f( 1.0f,-1.0f,-1.0f);		
  glEnd();					

}

float absf(float a) {
  return(a>=0?a:-a);
}

float min(float a,float b) {
  return(a<=b?a:b);
}

float sqr(float a) {
  return(a*a);
}

void DrawGLScene()
{
  
  float cameraX = 20.0*cos(cameraRotation);
  float cameraZ = 20.0*sin(cameraRotation);
  
  double stretch,height,position,rotation;
  
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);	

  glLoadIdentity ();
  
  gluLookAt (cameraX, cameraHeight,cameraZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    
  position=cos((double)frame*speed)*2;
  height=absf(sin((double)frame*speed)*10)+1;
  rotation=cos((double)frame*speed)*90;
  if(doTranslate) {
    if(height<2) stretch=height/2; else stretch=1;
  } else {
    stretch=sin((double)frame*speed/3)+1.5;
  }
  
  glPushMatrix();
  glTranslatef(0,-5,0);
  glScalef(8,0.2,8);
  drawCube();
  glPopMatrix();
  
  glPushMatrix();
  glTranslatef(2,-5,0);
  glScalef(1.2,-0.5,1.2);
  drawCube();
  glPopMatrix();
  
  glPushMatrix();
  glTranslatef(-2,-5,0);
  glScalef(1.2,-0.5,1.2);
  drawCube();
  glPopMatrix();
    
  if(doTranslate) glTranslatef(position,height-5,0.0);
  if(doRotate) glRotatef(rotation,0,1,0);
  if(doScale) glScalef(1/sqrt(stretch),stretch,1/sqrt(stretch));
  
  drawCube();
  
  frame++;
  cameraRotation += cameraRotationSpeed;
  cameraHeight = cameraHeight*0.99 + cameraHeightTarget*0.01;
   
  glutSwapBuffers();
}

void keyPressed(unsigned char key, int x, int y) 
{
  switch(key) {
    case 27:
      glutDestroyWindow(window); 
      exit(0);                   
    case 'r':
      doRotate = !doRotate;
      break;
    case 't':
      doTranslate = !doTranslate;
      break;
    case 's':
      doScale = !doScale;
      break;
    case '+':
      speed *= 1.1;
      frame = 0;
      break;
    case '-':
      speed /= 1.1;
      frame = 0;
      break;
    default:
      useMyOrtho = !useMyOrtho;
      ReSizeGLScene(windowWidth,windowHeight);
  }      
  DrawGLScene();
}

void specialKeyPressed(int key, int x, int y) 
{
  switch(key) {
    case GLUT_KEY_LEFT: 
      cameraRotationSpeed += 0.001;
      break;
    case GLUT_KEY_RIGHT: 
      cameraRotationSpeed -= 0.001;
      break;
    case GLUT_KEY_UP: 
      cameraHeightTarget += 1;
      break;
    case GLUT_KEY_DOWN: 
      cameraHeightTarget -= 1;
      break;
  }
  DrawGLScene();
}

int main(int argc, char **argv) 
{  
  glutInit(&argc, argv);  

  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);  
  glutInitWindowSize(640, 480);  
  glutInitWindowPosition(0, 0);  
  window = glutCreateWindow("OpenGL Framework");  

  glutDisplayFunc(&DrawGLScene);  
  glutIdleFunc(&DrawGLScene);
  glutReshapeFunc(&ReSizeGLScene);
  glutKeyboardFunc(&keyPressed);
  glutSpecialFunc(&specialKeyPressed);

  InitGL(640, 480);
  
  glutMainLoop();  

  return 1;
}




