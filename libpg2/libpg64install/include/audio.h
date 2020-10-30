#ifndef _AUDIO_H

#define _AUDIO_H

// intrnal functions
#ifdef LIBPG_IMPL
#include 	<SDL2/SDL_mixer.h>

void audio_process();
bool audio_open();
void audio_close();

#endif

#ifndef __cplusplus
#include 	<stdbool.h>
#endif



#ifdef __cplusplus 

extern "C" {
	
#endif

 
bool  sound_play(const char *path);
	 
void sound_pause();
	 
void sound_resume();
	 
void sound_stop();

	 
#ifdef __cplusplus 

}

	
#endif

#endif
