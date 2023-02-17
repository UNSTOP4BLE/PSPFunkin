#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdbool.h>   

static void Audio_Init(void) {}
static Mix_Music *Audio_LoadSong(const char *path) {return NULL;}
static Mix_Chunk *Audio_LoadSFX(const char *path) {return NULL;}
static void Audio_PlaySong(Mix_Music *music, bool loop) {}
static int Audio_GetSongMilli(Mix_Music *music) {return 0;}
static void Audio_PlaySFX(Mix_Chunk *audio, bool loop);
static void Audio_FreeSong(Mix_Music *music) {}
static void Audio_FreeSFX(Mix_Chunk *audio) {} 
static bool Audio_IsPlaying() {return false;}
