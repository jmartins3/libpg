CPP=g++ 
INCLUDES=
CPPFLAGS=-c -g -Wall

CC=gcc
CFLAGS=-c -g -std=c99 -Wall

all: battleship  battleship2

battleship: battleship.o board.o  components.o 
	$(CC) -o battleship board.o battleship.o  components.o  -lpg  -lm

battleship2: battleship2.o board.o  components.o comm.o strutils.o
	$(CC) -o battleship2 board.o battleship2.o  components.o comm.o strutils.o -lpg  -lm
	
# Compile rule	
.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
	
clean:
	rm -rf *.o battleship
