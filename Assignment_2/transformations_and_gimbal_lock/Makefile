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

all: transformations gimbal
transformations: main.o transformations.o
	$(CC) -o transformations main.o transformations.o $(LDFLAGS)
gimbal: gimbal.o
	$(CC) -o gimbal gimbal.o $(LDFLAGS)

clean:
	rm -f *.o
	rm -f transformations
	rm -f gimbal

transformations.o : transformations.h
transformations.o : transformations.h transformations.c
gimbal.o          : transformations.h gimbal.c
main.o            : transformations.h main.c
