INCLUDES = 
CC = gcc


CFLAGS = -g -Wall -std=c99

REGCLIENT2_SRCS = regsrv2.c 
REGCLIENT2_OBJS = $(REGSERVER_SRCS:.c=.o)


all: reg_client2   reg_stop reg_create_user reg_create_theme

reg_client2:	reg_client2.c
	$(CC) -o reg_client2  $(CFLAGS) reg_client2.c  -pthread

	
reg_stop:	reg_stop.c
	$(CC) -o reg_stop  $(CFLAGS) reg_stop.c  

reg_create_user:	reg_create_user.c
	$(CC) -o reg_create_user  $(CFLAGS) reg_create_user.c  

reg_create_theme:	reg_create_theme.c
	$(CC) -o reg_create_theme  $(CFLAGS) reg_create_theme.c  

# Compile rule	
.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
	

clean:
	rm -rf *.o reg_client2 reg_create_theme reg_stop reg_create_user
