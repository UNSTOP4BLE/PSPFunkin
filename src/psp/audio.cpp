#include "audio.h"
#include "../screen.h"

void Audio_Init(void)
{
    //Initialize all SDL subsystems
    if (SDL_Init(SDL_INIT_AUDIO) == -1)
    {
		ErrMSG("SDL_INIT FAILED");
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) != 0)
    {
		ErrMSG("FAILED TO OPEN AUDIO: %s\n", Mix_GetError());
    }
}

Mix_Music *Audio_LoadSong(const char *path)
{
    Mix_Music *music = Mix_LoadMUS(path);

    //If there was a problem loading the music
    if (music == NULL)
    {
        ErrMSG("FAILED TO FIND/LOAD MUSIC AT: %s", path);
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
        ErrMSG("FAILED TO FIND/LOAD SOUND AT: %s", path);
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

{
    Mix_FreeMusic(music);
}

void Audio_FreeSFX(Mix_Chunk *audio)
{
    Mix_FreeChunk(audio);
}

bool Audio_IsPlaying()
{
	return Mix_PlayingMusic();
}