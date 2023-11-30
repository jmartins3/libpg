
#include "../include/img_cache.h"
#include 	<map>
#include <string>

/**************************************************************
 * A cache to optimize image access on pglib
 * 
 * use STL map to map file names on SDL_Texture objects
 *
 **************************************************************/

using namespace std;
 
static map<string, SDL_Texture* > img_cache;


SDL_Texture*  icache_get(const char *img_path) {
    
    return img_cache[string(img_path)];
}

void icache_put(const char *img_path, SDL_Texture*  texture) {
    
    img_cache[string(img_path)] = texture;
}



