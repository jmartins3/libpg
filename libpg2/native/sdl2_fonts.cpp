#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

using std::cerr;
using std::endl;

int main(int argc, char* args[])
{
   // Initialize the SDL
   if (SDL_Init(SDL_INIT_VIDEO) != 0)
   {
      cerr << "SDL_Init() Failed: " <<
      SDL_GetError() << endl;
      exit(1);
   }

   // Set the video mode
   SDL_Window *display = SDL_CreateWindow("SDL Tutorial",
                          SDL_WINDOWPOS_UNDEFINED,
                          SDL_WINDOWPOS_UNDEFINED,
                          640, 480,
                          /*SDL_WINDOW_FULLSCREEN |  SDL_WINDOW_OPENGL */ 0);
   if (display == NULL)
   {
      cerr << "SDL_SetVideoMode() Failed: " <<        
      SDL_GetError() << endl;
      exit(1);
   }


   SDL_Renderer *renderer = SDL_CreateRenderer(display, -1, 0);
   // Initialize SDL_ttf library
   if (TTF_Init() != 0)
   {
      cerr << "TTF_Init() Failed: " << TTF_GetError() << endl;
      SDL_Quit();
      exit(1);
   }
   
   // Load a font
   TTF_Font *font;
   font = TTF_OpenFont("FreeSans.ttf", 25);
   if (font == NULL)
   {
      cerr << "TTF_OpenFont() Failed: " << TTF_GetError() << endl;
      TTF_Quit();
      SDL_Quit();
      exit(1);
   }

   // Write text to surface
   SDL_Surface *text;
   SDL_Texture *texture;
  
   SDL_Color text_color = {255, 255, 255};
   text = TTF_RenderText_Solid(font,
			"Teste" , //"A journey of a thousand miles begins with a single step.",
			text_color);

   if (text == NULL)
   {
      cerr << "TTF_RenderText_Solid() Failed: " << TTF_GetError() << endl;
      TTF_Quit();
      SDL_Quit();
      exit(1);
   }
   
   texture= SDL_CreateTextureFromSurface(renderer, text);

	int texW = 0;
	int texH = 0;
	SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
	SDL_Rect dstrect = { 100, 100, texW+100, texH +100};
   // Main loop
   SDL_Event event;
   while(1)
   {
      // Check for messages
      if (SDL_PollEvent(&event))
      {
         // Check for the quit message
         if (event.type == SDL_QUIT)
         {
            // Quit the program
            break;
         }
      }


      // Clear the screen, this was in SDL 1.1
      //if (SDL_FillRect(display, 
      //                 NULL,
      //                 SDL_MapRGB( display->format, 0,0,0))
      //                 != 0)
      //                 {
      //   cerr << "SDL_FillRect() Failed: " << SDL_GetError() << endl;
      //   break;
      //}

      // Apply the text to the display, this was in SDL 1.1
      // if (SDL_BlitSurface(text, NULL, display, NULL) != 0)
      // {
      //    cerr << "SDL_BlitSurface() Failed: " << SDL_GetError() << endl;
      //    break;
      // }
		
      SDL_RenderCopy(renderer, texture, NULL, &dstrect);
	  SDL_RenderPresent(renderer);

      //Update the display, this was in SDL 1.1
      //SDL_Flip(display);


   }

   // Shutdown the TTF library
   TTF_Quit();

   // Tell the SDL to clean up and shut down
   SDL_Quit();
    
   return 0;    
}
