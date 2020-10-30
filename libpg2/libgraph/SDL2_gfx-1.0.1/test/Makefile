# Makefile to build the SDL2_gfx tests

srcdir  = .

CC      = gcc
EXE	= 
CFLAGS  = -g -O2 -D_REENTRANT -I/usr/local/include/SDL2   -DHAVE_OPENGL -g
LIBS	=  -lSDL2_test -Wl,-rpath,/usr/local/lib -L/usr/local/lib -lSDL2 -lpthread   -lSDL2_gfx 

TARGETS = \
	testgfx$(EXE) \
	testrotozoom$(EXE) \
	testimagefilter$(EXE) \

all: Makefile $(TARGETS)

Makefile: $(srcdir)/Makefile.in
	$(SHELL) config.status $@

testgfx$(EXE): $(srcdir)/testgfx.c
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS) -lm

testrotozoom$(EXE): $(srcdir)/testrotozoom.c
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS) -lm

testimagefilter$(EXE): $(srcdir)/testimagefilter.c
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS) -lm

clean:
	rm -f $(TARGETS)
	rm -f *~
	rm -f *.user
	rm -f *.stackdump

distclean: clean
	rm -f Makefile
	rm -f config.status config.cache config.log
	rm -rf $(srcdir)/autom4te*
	rm -rf TestGfx TestRotozoom TestImageFilter Win32
	

