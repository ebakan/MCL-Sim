CC=gcc
TARGET=mcl
DEBUG=-g
LFLAGS=-std=c99 -Wall $(DEBUG)
CFLAGS=-std=c99 -Wall $(DEBUG) -c
FRAMEWORKS=-framework OpenGL -framework GLUT
DEPS=gui.o lookups.o mcl.o map.o rand.o gsl/gsl.a

all: $(TARGET)

$(TARGET) : main.c $(DEPS)
	    $(CC) $(LFLAGS) main.c -o $(TARGET) $(FRAMEWORKS) $(DEPS)

gui.o : gui.c gui.h mcl.h structs.h
	$(CC) $(CFLAGS) $<

lookups.o : lookups.c lookups.h mcl.h
	$(CC) $(CFLAGS) $<

mcl.o : mcl.c mcl.h structs.h
	$(CC) $(CFLAGS) $<

map.o : map.c map.h
	$(CC) $(CFLAGS) $<

rand.o : rand.c rand.h gsl/gsl.a gsl/gsl.h
	$(CC) $(CFLAGS) $<

gsl/gsl.a : FORCE_MAKE
	cd gsl && make

FORCE_MAKE :
	true

.IGNORE .PHONY clean :
	rm $(TARGET)
	rm *.o
	rm -r $(TARGET).dSYM
	cd gsl && make clean
