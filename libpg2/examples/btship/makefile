CPP=g++ 
INCLUDES=
CPPFLAGS=-c -g -Wall

CC=gcc
CFLAGS=-c -g -std=c99 -Wall

all: btship   

btship: btship.o board.o  strutils.o
	$(CC) -o btship  btship.o board.o  strutils.o -lpg  -lm

 
	
# Compile rule	
.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
	
clean:
	rm -rf *.o  btship 
