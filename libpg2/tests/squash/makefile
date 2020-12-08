 
CC=gcc
CFLAGS=-c -g -Wall
INCLUDES=squash.h ball.h racket.h 7seg.h counter.h

all: squash

squash:	squash.o ball.o counter.o racket.o 7seg.o
	gcc -o squash -Wall squash.o ball.o racket.o counter.o 7seg.o -lpg
	

squash.o: squash.c $(INCLUDES)
	gcc -c squash.c
	
ball.o: ball.c $(INCLUDES)
	gcc -c ball.c
	
counter.o: counter.c $(INCLUDES)
	gcc -c counter.c
	
racket.o: racket.c $(INCLUDES)
	gcc -c racket.c
	
7seg.o: 7seg.c $(INCLUDES)
	gcc -c 7seg.c
	
		
clean:
	rm -rf *.o squash
