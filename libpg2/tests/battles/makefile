CPP=g++ 
INCLUDES=
CPPFLAGS=-c -g -Wall

CC=gcc
CFLAGS=-c -g -std=c99 -Wall

all: battleship  

battleship: battleship.o board.o 
	$(CC) -o battleship board.o battleship.o   -lpg  
	
# Compile rule	
.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
	
clean:
	rm -rf *.o battleship
