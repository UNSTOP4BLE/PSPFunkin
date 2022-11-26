#ifndef __WAV_H__
#define __WAV_H__
#include <stdbool.h>
typedef struct
{
        unsigned long channels; /**<  Number of channels */
        unsigned long sampleRate; /**<  Sample rate */
        unsigned long sampleCount; /**<  Sample count */
        unsigned long dataLength; /**<  Data length */
        unsigned long rateRatio; /**<  Rate ratio (sampleRate / 44100 * 0x10000) */
        unsigned long playPtr; /**<  Internal */
        unsigned long playPtr_frac; /**<  Internal */
        int loop; /**<  Loop flag */
        void *data; /**< A pointer to the actual WAV data */
        int id; /**<  The ID of the WAV */
        unsigned long bitPerSample; /**<  The bit rate of the WAV */
		int slot;
} Wav;

bool Wav_Init();
Wav *Wav_Load(const char *filename);
void Wav_Free(Wav *theWav);
bool Wav_Play(Wav *theWav);
void Wav_Stop(Wav *theWav);
void Wav_StopAll();
void Wav_SetLoop(Wav *theWav, int loop);
bool Wav_Playing();
int Wav_GetTime(Wav *theWav);

#endif 
