#include <SDL2/SDL_image.h>


#ifdef __cplusplus 

extern "C" {
	
#endif

SDL_Texture*  icache_get(const char *img_path);
    

void icache_put(const char *img_path, SDL_Texture*  texture);
    

#ifdef __cplusplus 
}
#endif
