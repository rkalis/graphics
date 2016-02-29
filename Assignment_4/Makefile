CC=gcc

UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)
	LDFLAGS=-g -framework opengl -framework glut
	CFLAGS=-g -O3 -std=c99 $(WARNING_FLAGS) -fno-strict-aliasing -D OS_X
else
	LDFLAGS=-g -lGL -lglut -lGLU -lm
	CFLAGS=-g -O3 -std=c99 $(WARNING_FLAGS) -fno-strict-aliasing
endif

WARNING_FLAGS=-Wall -Wextra -Werror-implicit-function-declaration -Wshadow -Wstrict-prototypes -pedantic-errors

.c.o:
	$(CC) -c $(CFLAGS) $<

all: main
main: bbox.o bvh.o intersection.o main.o perlin.o plymodel.o quat.o rply.o scene.o shaders.o
	$(CC) -o main bbox.o bvh.o intersection.o main.o perlin.o plymodel.o quat.o rply.o scene.o shaders.o $(LDFLAGS)

clean:
	rm -f *.o
	rm -f main

shaders.o      : constants.h intersection.h perlin.h quat.h scene.h shaders.h v3math.h shaders.c
shaders.o      : types.h shaders.h
perlin.o       : perlin.h
main.o         : bvh.h constants.h intersection.h perlin.h plymodel.h scene.h shaders.h v3math.h main.c
perlin.o       : perlin.h perlin.c
rply.o         : rply.h rply.c
v3math.o       : v3math.h
quat.o         : v3math.h quat.h
rply.o         : rply.h
quat.o         : constants.h quat.h quat.c
scene.o        : bvh.h plymodel.h scene.h scene.c
scene.o        : types.h scene.h
types.o        : v3math.h types.h
bvh.o          : bvh.h scene.h v3math.h bvh.c
plymodel.o     : plymodel.h rply.h v3math.h plymodel.c
plymodel.o     : types.h plymodel.h
bvh.o          : bbox.h bvh.h
constants.o    : types.h constants.h
intersection.o : types.h intersection.h
intersection.o : bvh.h constants.h intersection.h scene.h v3math.h intersection.c
bbox.o         : bbox.h constants.h v3math.h bbox.c
bbox.o         : types.h bbox.h
