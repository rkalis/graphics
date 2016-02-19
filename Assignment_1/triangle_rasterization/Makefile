CC=gcc
UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)
	WARNING_FLAGS=-Wall -Wextra -Werror-implicit-function-declaration -Wshadow -Wstrict-prototypes -pedantic-errors
	CFLAGS=-g -O2 -std=c99 $(WARNING_FLAGS) -D OS_X
	LDFLAGS=-g -framework opengl -framework glut
else
	WARNING_FLAGS=-Wall -Wextra -Werror-implicit-function-declaration -Wshadow -Wstrict-prototypes -pedantic-errors
	CFLAGS=-g -O2 -std=c99 $(WARNING_FLAGS)
	LDFLAGS=-g -lGL -lglut -lGLU -lm
endif

.c.o:
	$(CC) -c $(CFLAGS) $<

all: trirast
trirast: main.o trirast.o
	$(CC) -o trirast main.o trirast.o $(LDFLAGS)

clean:
	rm -f *.o
	rm -f trirast

trirast.o   : types.h trirast.c
types.o     : types.h
main.o      : triangles.h trirast.h types.h main.c
triangles.o : triangles.h triangles.h
trirast.o   : types.h trirast.h
