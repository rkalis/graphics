CC=gcc
CFLAGS=$(shell sdl-config --cflags) -Wall -std=c99 -pedantic-errors
LDFLAGS=$(shell sdl-config --libs) -L/usr/X11R6/lib -lm -lGL -lGLU -lglut
#SOURCES=$(shell ls *.c *.c??)

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
