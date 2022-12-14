#include "audio.h"
#include "../error.h"
#include "../game.h"

void Audio_Init(void)
{
    //Initialize all SDL subsystems
    SDL_Init(SDL_INIT_AUDIO);
    Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096);
}

Mix_Music *Audio_LoadSong(const char *path)
{
    Mix_Music *music = Mix_LoadMUS(path);

    //If there was a problem loading the music
    if (music == NULL)
    {
		sprintf(message, "FAILED TO FIND/LOAD MUSIC AT: %s", path);
		game.gamestate = 4;
		return NULL;
    }
    return music;
}

Mix_Chunk *Audio_LoadSFX(const char *path)
{   
    //Load the sound effects
    Mix_Chunk *audio = Mix_LoadWAV(path);

    //If there was a problem loading the sound effects
    if (audio == NULL)
    {
		sprintf(message, "FAILED TO FIND/LOAD SOUND AT: %s", path);
		game.gamestate = 4;
		return NULL;
    }
    return audio;
}

void Audio_PlaySong(Mix_Music *music, bool loop)
{
	Mix_PlayMusic(music, loop);
}

int Audio_GetSongMilli(Mix_Music *music)
{
	return Mix_GetMusicPosition(music) * 1000;
}

void Audio_PlaySFX(Mix_Chunk *audio, bool loop)
{
 	Mix_PlayChannel(-1, audio, loop);
}

bool Audio_IsPlaying()
{
	return Mix_PlayingMusic();
}