
/*-----------------------------------------------------------
 * This module supports basic audio operations for pcm sound files (.wav)
 *----------------------------------------------------------*/


#define 	LIBPG_IMPL

#include 	<sys/times.h>
#include 	<map>
#include 	"../include/graphics.h"
#include  	<stdbool.h>
#include  	<signal.h>
#include 	"../include/audio.h"

using namespace std;

/*-------------------------------------
 * Globals
 *------------------------------------*/


static Mix_Chunk  *music;
static bool opened = false;

bool  sound_play(const char *path) {
	Mix_Chunk  *musicaux;
	
	audio_open();
    musicaux = Mix_LoadWAV(path);
    if (musicaux == NULL) {
		fprintf(stderr, "loading music error: %s\n", Mix_GetError());
		audio_close();
		return false;
	}
    if ( Mix_PlayChannel(-1, musicaux, 0) == -1 ) {
		printf("Play music error: %s\n", Mix_GetError());
		Mix_FreeChunk(musicaux);
		audio_close();
		return false;
	}
	
    if (music != NULL) {
		Mix_FreeChunk(music);
	}
	music = musicaux;
	return true;
}


void sound_pause() {
	if (music != NULL && Mix_Playing(-1)  )
		Mix_Pause(-1);
}

void sound_resume() {
	if (music != NULL && Mix_Playing(-1) && Mix_Paused(-1) )
		Mix_Resume(-1);
}

void sound_stop() {
	audio_close();
}


// free adio resources if music stopped
void audio_process() {
	if (music != NULL && Mix_Playing(-1) == 0  ) {
		audio_close();
	}
}

bool audio_open() {
	if (!opened) {
		if (Mix_OpenAudio(11025, MIX_DEFAULT_FORMAT, 2, 4096) == -1) {
				 fprintf(stderr, "Could not open audio mixer, error:%s\n", Mix_GetError());
				 return false;
		}
		opened=true;
	}
	return true;
}

// free audio resources
void audio_close() {
	if (music != NULL ) {
			Mix_FreeChunk(music);
			music=NULL;
	}
	if (opened) {
		opened=false;
		Mix_CloseAudio();
	}
}

	
 	 



