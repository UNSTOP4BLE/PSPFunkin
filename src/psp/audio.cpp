#include "audio.h"
#include "../error.h"
#include "../game.h"

void Audio_Init(void)
{
    //Initialize all SDL subsystems
    SDL_Init(SDL_INIT_AUDIO);
    Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096);
}

void Audio_LoadSong(Mix_Music *music, const char *path)
{
    music = Mix_LoadMUS(path);

    //If there was a problem loading the music
    if (music == NULL)
    {
		sprintf(message, "FAILED TO FIND MUSIC AT: %s", path);
		game.gamestate = 4;
		return;
    }
}

void Audio_LoadSFX(Mix_Chunk *audio, const char *path)
{   
    //Load the sound effects
    audio = Mix_LoadWAV(path);

    //If there was a problem loading the sound effects
    if (audio == NULL)
    {
		sprintf(message, "FAILED TO FIND SOUND AT: %s", path);
		game.gamestate = 4;
		return;
    }
}

void Audio_PlaySong(Mix_Music *music, bool loop)
{
	Mix_PlayMusic(music, loop);
}

int Audio_GetSongMilli(Mix_Chunk *music)
{
//	return Mix_GetMusicPosition(music) * 1000;
}

void Audio_PlaySFX(Mix_Chunk *audio, bool loop)
{
 	Mix_PlayChannel(-1, audio, loop);
}

void Audio_FreeSong(Mix_Music *music)
{
    Mix_FreeMusic(music);
}

void Audio_FreeSFX(Mix_Chunk *audio)
{
    Mix_FreeChunk(audio);
}