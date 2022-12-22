#ifndef __AUDIO_H__
#define __AUDIO_H__

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

void Audio_Init(void);
Mix_Music *Audio_LoadSong(const char *path);
Mix_Chunk *Audio_LoadSFX(const char *path);
void Audio_PlaySong(Mix_Music *music, bool loop);
int Audio_GetSongMilli(Mix_Music *music);
void Audio_PlaySFX(Mix_Chunk *audio, bool loop);
void Audio_FreeSong(Mix_Music *music);
void Audio_FreeSFX(Mix_Chunk *audio);

#endif
