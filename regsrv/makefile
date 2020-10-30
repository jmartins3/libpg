INCLUDES = 
CC = gcc


CFLAGS = -g -Wall

REGSERVER_SRCS = regsrv2.c channel.c commands.c responses.c repository.c strutils.c
REGSERVER_OBJS = $(REGSERVER_SRCS:.c=.o)


all: regsrv2  

regsrv2:	$(REGSERVER_OBJS)
	$(CC) -o regsrv2   $(REGSERVER_OBJS) -pthread -luv -ldl
 

# Compile rule	
.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
	

clean:
	rm -rf *.o regsrv2  
	
