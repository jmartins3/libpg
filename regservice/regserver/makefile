INCLUDES = 
CC = gcc
CPP = g++

CFLAGS = -g -Wall

REGSERVER_SRCS = regsrv.c channel.c commands.c responses.c repository.c strutils.c 
REGSERVER_CPPSRCS = activeusers.cpp
REGSERVER_OBJS = $(REGSERVER_SRCS:.c=.o) 
REGSERVER_CPPOBJS = $(REGSERVER_CPPSRCS:.cpp=.o) 


 
SESSION_TEST_CPPSRCS = sessions_test.cpp   activeusers.cpp mock_topics.c
SESSION_TEST_CPPOBJS =  $(SESSION_TEST_CPPSRCS:.cpp=.o) 

all: regsrv sessions_test

regsrv:	$(REGSERVER_OBJS) $(REGSERVER_CPPOBJS)
	$(CPP) -o regsrv $(REGSERVER_OBJS) $(REGSERVER_CPPOBJS) -pthread -luv -ldl

sessions_test: $(SESSION_TEST_CPPOBJS)
	$(CPP) -o sessions_test $(SESSION_TEST_CPPOBJS) -pthread -luv

# Compile rule	
.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
.cpp.o:
	$(CPP) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf *.o regsrv  
	
