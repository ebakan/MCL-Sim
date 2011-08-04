CC=gcc
TARGET=gsl.a
DEBUG=-g
LFLAGS=-std=c99 -Wall $(DEBUG)
CFLAGS=-std=c99 -Wall $(DEBUG) -c
DEPS=default.o gausszig.o gsl.o rng.o taus.o types.o

all: $(TARGET)

$(TARGET) : gsl.h $(DEPS)
	ar -cr $(TARGET) $(DEPS)

%.o : %.c gsl.h
	$(CC) $(CFLAGS) $<

.IGNORE .PHONY clean :
	rm *.o
	rm $(TARGET)
