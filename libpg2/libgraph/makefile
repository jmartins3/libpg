CPP=g++ 
INC=/usr/local/include/SDL2

CPPFLAGS=-c -g -I$(INC) -Wall   -fPIC

all: graphics.o  events.o audio.o triangles.o graphics_text.o comm.o hooks.o socket_events.o \
	uv_srv.o components.o strutils.o

triangles.o: triangles.cpp
	$(CPP) $(CPPFLAGS) triangles.cpp
	
graphics.o: graphics.cpp
	$(CPP) $(CPPFLAGS) graphics.cpp

events.o: events.cpp
	$(CPP) $(CPPFLAGS) events.cpp

comm.o: comm.cpp
	$(CPP) $(CPPFLAGS) comm.cpp
	
strutils.o: strutils.cpp
	$(CPP) $(CPPFLAGS) strutils.cpp
	
audio.o: audio.cpp
	$(CPP) $(CPPFLAGS) audio.cpp

graphics_text.o: graphics_text.cpp
	$(CPP) $(CPPFLAGS) graphics_text.cpp

socket_events.o: socket_events.cpp
	$(CPP) $(CPPFLAGS) socket_events.cpp
	
uv_srv.o: uv_srv.cpp
	$(CPP) $(CPPFLAGS) uv_srv.cpp

	
components.o: components.cpp
	$(CPP) $(CPPFLAGS) components.cpp
	
hooks.o: hooks.cpp
	$(CPP) $(CPPFLAGS) hooks.cpp	
	
clean:
	rm -rf  *.o  
