INCLUDES = 
CC = gcc


CFLAGS = -g -Wall -std=c99


all: reg_client2   reg_stop reg_create_user reg_create_theme reg_list_themes \
	 reg_create_topic reg_list_topics

reg_client2:	reg_client2.c
	$(CC) -o reg_client2  $(CFLAGS) reg_client2.c  -pthread

	
reg_stop:	reg_stop.c reg_client_utils.c
	$(CC) -o reg_stop  $(CFLAGS) reg_stop.c   reg_client_utils.c

reg_create_user:	reg_create_user.c reg_client_utils.c
	$(CC) -o reg_create_user  $(CFLAGS) reg_create_user.c  reg_client_utils.c

reg_create_theme:	reg_create_theme.c reg_client_utils.c
	$(CC) -o reg_create_theme  $(CFLAGS) reg_create_theme.c   reg_client_utils.c

reg_list_themes:	reg_list_themes.c reg_client_utils.c
	$(CC) -o reg_list_themes  $(CFLAGS) reg_list_themes.c   reg_client_utils.c

reg_create_topic:	reg_create_topic.c reg_client_utils.c
	$(CC) -o reg_create_topic  $(CFLAGS) reg_create_topic.c   reg_client_utils.c

reg_list_topics:	reg_list_topics.c reg_client_utils.c
	$(CC) -o reg_list_topics  $(CFLAGS) reg_list_topics.c   reg_client_utils.c
		
# Compile rule	
.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
	

clean:
	rm -rf *.o reg_client2 reg_create_theme reg_stop reg_create_user reg_list_themes \
	reg_create_topic reg_list_topics
