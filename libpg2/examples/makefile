

CC=gcc
CFLAGS= -g -Wall

all: graphics_test eventstestnew figure eventstest


graphics_test: graphics_test.o
	$(CC) -o graphics_test $(CFLAGS)  graphics_test.o   -lpg   
	
eventstestnew: eventstestnew.c
	$(CC) -o eventstestnew  $(CFLAGS) eventstestnew.c -lpg

eventstest: eventstest.c
	$(CC) -o eventstest   $(CFLAGS) eventstest.c -lpg


figure: figure.c
	$(CC)  -o figure $(CFLAGS) figure.c -lpg
	
clean:
	rm -rf  *.o  eventstestnew eventstest graphics_test  figure
