CPP=g++ 
INC=/usr/local/include/SDL2
CPPFLAGS=-c -g -I$(INC) -Wall

CC=gcc
CFLAGS=-c -g -Wall

all: showLetters sockets 4inline

showLetters: showLetters.o components.o utils.o
	$(CPP) -o showLetters showLetters.o components.o utils.o -lpg -lm -luv
	
showLetters.o: showLetters.cpp
	$(CPP) $(CPPFLAGS) showLetters.cpp
	
components.o: components.c
	$(CC) $(CFLAGS) components.c

utils.o: utils.c
	$(CC) $(CFLAGS) utils.c

sockets: sockets.o
	$(CC) -o sockets sockets.o   -lpg  -luv
	
sockets.o: sockets.c
	$(CC) $(CFLAGS) sockets.c

4inline: 4inline.c components.c utils.c
	$(CPP) -o 4inline -Wall 4inline.c components.o utils.o -lpg -lm -luv
	
clean:
	rm -rf  *.o  
