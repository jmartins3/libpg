CPP=g++ 
INCLUDES=
CPPFLAGS=-c -g -Wall

CC=gcc
CFLAGS=-c -g -std=c99 -Wall

all: btship2   

btship2: battleship2.o board.o  comm.o strutils.o
	$(CC) -o btship2  battleship2.o board.o   comm.o strutils.o -lpg  -lm

 
	
# Compile rule	
.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
	
clean:
	rm -rf *.o  btship2 btship
