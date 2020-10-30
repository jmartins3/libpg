INCLUDES = 
CC = gcc


CFLAGS = -g -Wall

REGSERVER_SRCS = regsrv.c channel.c commands.c responses.c repository.c strutils.c
REGSERVER_OBJS = $(REGSERVER_SRCS:.c=.o)


all: regsrv 

regsrv:	$(REGSERVER_OBJS)
	$(CC) -o regsrv   $(REGSERVER_OBJS) -pthread -luv -ldl
 

# Compile rule	
.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
	

clean:
	rm -rf *.o regsrv  
	
