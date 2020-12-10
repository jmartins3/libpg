
BIN=bin
LIBS= -lpthread -Wl,-rpath,/usr/local/lib -lSDL2_gfx -lSDL2_image  -lSDL2_mixer -lSDL2 -luv 


OBJS= libgraph/graphics.o libgraph/events.o libgraph/triangles.o \
	libgraph/audio.o  libgraph/graphics_text.o libgraph/hooks.o \
	libgraph/socket_events.o libgraph/uv_srv.o  libgraph/components.o


all:
	cd libgraph; make
	#cd libjson; make
	g++ -o $(BIN)/libpg.so -shared -rdynamic $(OBJS) $(LIBS)
	 
 	

 
cleanall:
	cd libgraph; make clean
	cd libjson; make clean
	rm -rf *.so

clean:
	rm -rf *.so		


