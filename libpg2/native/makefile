
all:  sdl2f ftest2

 
	
sdl2f: sdl2_fonts.o
	g++ -o sdl2f sdl2_fonts.o  -lSDL2_ttf -lSDL2

sdl2_fonts.o: sdl2_fonts.cpp
	g++ -c -Wall sdl2_fonts.cpp
	
ftest2: ftest2.o
	g++ -o ftest2 ftest2.o -lSDL2 -lSDL2_ttf

ftest2.o: ftest2.cpp
	g++ -c ftest2.cpp
