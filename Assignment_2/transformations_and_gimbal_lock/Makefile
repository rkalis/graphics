CC=gcc

WARNING_FLAGS=-Wall -Wextra -Werror-implicit-function-declaration -Wshadow -Wstrict-prototypes -pedantic-errors
CFLAGS=-O2 -std=c99 $(WARNING_FLAGS)
LDFLAGS=-lGL -lglut -lGLU -lm
#LDFLAGS=-lglut -lglu -lgl

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
