#add new comment to makefile just for test purposes!

BIN=bin
LIBS= -lpthread -Wl,-rpath,/usr/local/lib -lSDL2_gfx -lSDL2_image  -lSDL2_mixer -lSDL2 -luv 


OBJS= libgraph/graphics.o libgraph/events.o libgraph/triangles.o \
	libgraph/audio.o  libgraph/graphics_text.o libgraph/hooks.o \
	libgraph/socket_events.o libgraph/uv_srv.o  libgraph/components.o \
	libgraph/comm.o libgraph/strutils.o libgraph/img_cache.o



all:
	cd libgraph; make
	#cd libjson; make
	g++ -o $(BIN)/libpg.so -shared -rdynamic $(OBJS) $(LIBS)
 	

prepare:
	chmod 755 cr_bin
	./cr_bin
	
 
cleanall:
	cd libgraph; make clean
	cd libjson; make clean
	rm -rf *.so

clean:
	rm -rf *.so		


deploy:
	rm -rf libpg64install/include/*
	cp include/* libpg64install/include/
	cp bin/libpg.so libpg64install
	cd libpg64install; sudo ./libinstall
	
