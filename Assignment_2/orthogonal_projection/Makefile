#Makefile for assignment 5 of CG 2006

CC=gcc
CFLAGS=$(shell sdl-config --cflags) -Wall -std=c99 -pedantic-errors
LDFLAGS=$(shell sdl-config --libs) -L/usr/X11R6/lib -lm -lGL -lGLU -lglut -lXi -lXmu
#SOURCES=$(shell ls *.c *.c??)

all: main

main: main.o ortho.o
	$(CC) -o main main.o ortho.o $(CFLAGS) $(LDFLAGS)

main.o: main.c ortho.h
	$(CC) $(CFLAGS) -c -o main.o main.c

ortho.o: ortho.h ortho.c
	$(CC) $(CFLAGS) -c -o ortho.o ortho.c

#does vi make any backups, like emacs does?
realclean: clean
	rm -f *~

clean:
	rm -f *.o
	rm -f main
