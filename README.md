# Graphics & Game Technology

These are our university assignments for the course on Computer Graphics.

The code and Makefiles should platform independent on Unix platforms and have been tested on OSX 10.11 and Ubuntu 14.04.

Most assignments are written in C with some [OpenGL 2.0](https://www.opengl.org/). Assigment 1 makes use of [SDL](https://www.libsdl.org/), Assignment 3 makes use of the [Bullet](http://bulletphysics.org/) physics library, and assignment 6 is written fully using C# and [Unity3D](http://unity3d.com/).

Assignment 1:
- Bresenham's midpoint line algorithm with (Does not work on OS X due to outdated SDL)
- Triangle Rasterisation using Barycentric coordinates

Assignment 2:
- Orthoganal Projections: Implementing OpenGL ortho
- Moving the camera: Implementing OpenGL lookAt
- Transformations: Implementing OpenGL scale, translate, rotate
- Experiencing Gimbal Locks

Assignment 3:
- Bezier Curves: Implementing Bezier curves with C and using them to control a robot arm (Robot control does not work on OS X due to incompatible physics library)

Assigment 4:
- Implementing Shaders using raytracing
  - Matte Shader with shadows
  - Blinn-Phong Shader - Matte shader with a specular element
  - Reflection Shader - Blinn-Phong shader with reflective surfaces

Assignment 5:
- Isosurfaces: Implementing the Marching Tetrahedra algorithm
- Texture mapping: Specifying Texture Coordinates to map textures to objects in a scene, also implementing functions for texture mapping of spheres, cilinders and simple polygons

Assignment 6:
- Creating a simple platformer game Using Unity3D, more information can be found in the Assignment 6 README
