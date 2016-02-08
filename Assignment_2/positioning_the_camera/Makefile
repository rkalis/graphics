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

all: main

main: main.o lookat.o
	$(CC) -o main main.o lookat.o $(CFLAGS) $(LDFLAGS)

main.o: main.c lookat.h
	$(CC) $(CFLAGS) -c -o main.o main.c

lookat.o: lookat.h lookat.c
	$(CC) $(CFLAGS) -c -o lookat.o lookat.c

#does vi make any backups, like emacs does?
realclean: clean
	rm -f *~

clean:
	rm -f *.o
	rm -f main
