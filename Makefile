CC = gcc
CFLAGS = -Wall -g -std=c99 -pedantic

OBJECTS = spoder.o utilities.o socket.o

.PHONY: all clean

all: spoder


spoder: $(OBJECTS)
	$(CC) -o $@ $^

%.o: %.c
	$(CC) -c -o $@ $<

spoder.o: spoder.c utilities.h
socket.o: socket.c socket.h utilities.h
utilities.o: utilities.c utilities.h


clean:
	rm -rf *.o spoder *.txt