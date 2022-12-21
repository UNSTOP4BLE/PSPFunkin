#ifndef __AUDIO_H__
#define __AUDIO_H__

#include "SDL/SDL.h"
#include "SDL/SDL_mixer.h"

void Audio_Init(void);
void Audio_LoadSong(Mix_Music *music, const char *path);
void Audio_LoadSFX(Mix_Chunk *audio, const char *path);
void Audio_PlaySong(Mix_Chunk *music, bool loop);
int Audio_GetSongMilli(Mix_Chunk *music);
void Audio_PlaySFX(Mix_Chunk *audio, bool loop);
void Audio_FreeSong(Mix_Music *music);
void Audio_FreeSFX(Mix_Chunk *audio);

#endif
