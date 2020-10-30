INCLUDES = 
CC = gcc


CFLAGS = -g -Wall -std=c99

REGCLIENT2_SRCS = regsrv2.c 
REGCLIENT2_OBJS = $(REGSERVER_SRCS:.c=.o)


all: reg_client2 reg_client2a   reg_stop create_user

reg_client2:	reg_client2.c
	$(CC) -o reg_client2  $(CFLAGS) reg_client2.c  -pthread

reg_client2a:	reg_client2a.c
	$(CC) -o reg_client2a  $(CFLAGS) reg_client2a.c  -pthread
	
reg_stop:	reg_stop.c
	$(CC) -o reg_stop  $(CFLAGS) reg_stop.c  

create_user:	create_user.c
	$(CC) -o create_user  $(CFLAGS) create_user.c  

# Compile rule	
.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
	

clean:
	rm -rf *.o reg_client2 reg_client2a   reg_stop create_user
