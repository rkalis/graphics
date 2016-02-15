CC=gcc
CXX=g++

#BULLET=./bullet32

UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)
	BULLET=./bullet64MAC
	LDFLAGS=-g -L${BULLET}/lib -framework opengl -framework glut
	CFLAGS=-g -O2 -std=c99 $(WARNING_FLAGS) -fno-strict-aliasing -D OS_X
	CXXFLAGS=-g -O2 -Wall -Wextra -I${BULLET}/include -D OS_X
else
	BULLET=./bullet64
	LDFLAGS=-g -L${BULLET}/lib -lGL -lglut -lGLU
	CFLAGS=-g -O2 -std=c99 $(WARNING_FLAGS) -fno-strict-aliasing
	CXXFLAGS=-g -O2 -Wall -Wextra -I${BULLET}/include
endif
	
WARNING_FLAGS=-Wall -Wextra -Werror-implicit-function-declaration -Wshadow -Wstrict-prototypes -pedantic
LIBS=-lBulletDynamics -lBulletCollision -lLinearMath

.c.o:
	$(CC) -c $(CFLAGS) $<
.cpp.o:
	$(CXX) -c $(CXXFLAGS) $<

all: singlecurve multicurve
singlecurve: singlecurve.o bezier.o
	$(CC) -o singlecurve singlecurve.o bezier.o $(LDFLAGS)
multicurve: multicurve.o bezier.o v3math.o plymodel.o rply.o physics.o
	$(CXX) -o multicurve multicurve.o bezier.o v3math.o plymodel.o rply.o physics.o $(LDFLAGS) $(LIBS)

clean:
	rm -f *.o
	rm -f singlecurve
	rm -f multicurve

bezier.o        : bezier.h bezier.c
singlecurve.o   : bezier.h singlecurve.c
multicurve.o    : bezier.h multicurve.c plymodel.h physics.h
v3math.o        : v3math.h v3math.c
plymodel.o      : plymodel.c plymodel.h
rply.o          : rply.c rply.h
physics.o       : physics.h
