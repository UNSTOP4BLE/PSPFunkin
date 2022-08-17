#include <string.h>
#include <stdbool.h>
#include <pspthreadman.h>
#include <pspaudio.h>

#include "audio.h"

static int AudioReady = 0;
static signed short AudioSoundBuffer[_NUM_AUDIO_CHANNELS][2][_NUM_AUDIO_SAMPLES][2];

static AudioChannelInfo AudioStatus[_NUM_AUDIO_CHANNELS];

static volatile int AudioTerminate = 0;

void AudioSetVolume(int channel, int left, int right)
{
	AudioStatus[channel].volumeRight = right;
	AudioStatus[channel].volumeLeft  = left;
}


void AudioSetChannelCallback(int channel, AudioCallback callback, void *data)
{
	volatile AudioChannelInfo *pci = &AudioStatus[channel];
	
	if (callback == 0)
		pci->callback = 0;
	else
	{
		pci->callback = callback;
		sceKernelWakeupThread(pci->threadHandle);
	}
}


int AudioOutBlocking(int channel, int left, int right, void *data)
{
	if (!AudioReady) return(-1);
	if (channel >= _NUM_AUDIO_CHANNELS) return(-1);
	if (left > _VOLUME_MAX) left = _VOLUME_MAX;
	if (right > _VOLUME_MAX) right = _VOLUME_MAX;

	return sceAudioOutputPannedBlocking(AudioStatus[channel].handle, left, right, data);
}


static int AudioChannelThread(int args, void *argp)
{
	volatile int bufidx = 0;
	int channel = *(int *) argp;
	
	while (AudioTerminate == 0)
	{
		void *bufptr = &AudioSoundBuffer[channel][bufidx];
		AudioCallback callback;
		callback = AudioStatus[channel].callback;

		if (callback)
		{
			callback(bufptr, _NUM_AUDIO_SAMPLES, AudioStatus[channel].data);
			AudioOutBlocking(channel, AudioStatus[channel].volumeLeft, AudioStatus[channel].volumeRight, bufptr);
		} 
		else
		{
			sceKernelSleepThread();
		}

		bufidx = (bufidx ? 0:1);
	}

	sceKernelExitThread(0);

	return(0);
}

bool AudioInit(int priority)
{
	int i, ret;
	int failed = 0;
	char str[32];

	AudioTerminate = 0;
	AudioReady = 0;
	
	for (i = 0; i < _NUM_AUDIO_CHANNELS; i++)
	{
		AudioStatus[i].handle = -1;
		AudioStatus[i].threadHandle = -1;
		AudioStatus[i].volumeRight = _VOLUME_MAX;
		AudioStatus[i].volumeLeft  = _VOLUME_MAX;
		AudioStatus[i].callback = 0;
		AudioStatus[i].data = 0;
	}

	for (i = 0; i < _NUM_AUDIO_CHANNELS; i++)
	{
		if ((AudioStatus[i].handle = sceAudioChReserve(-1, _NUM_AUDIO_SAMPLES, 0)) < 0)
		failed = 1;
	}
	
	if (failed)
	{
		for (i = 0; i < _NUM_AUDIO_CHANNELS; i++)
		{
			if (AudioStatus[i].handle != -1)
				sceAudioChRelease(AudioStatus[i].handle);

			AudioStatus[i].handle = -1;
		}

		return(0);
	}
	
	AudioReady = 1;

	strcpy(str, "Audio0");
	
	for (i = 0; i < _NUM_AUDIO_CHANNELS; i++)
	{
		str[8] = '0' + i;
		AudioStatus[i].threadHandle = sceKernelCreateThread(str, (void*)&AudioChannelThread, priority, 0x10000, PSP_THREAD_ATTR_USER, NULL);
		
		if (AudioStatus[i].threadHandle < 0)
		{
			AudioStatus[i].threadHandle = -1;
			failed = 1;
			break;
		}

		ret = sceKernelStartThread(AudioStatus[i].threadHandle, sizeof(i), &i);

		if (ret != 0)
		{
			failed = 1;
			break;
		}
	}

	if (failed)
	{
		AudioTerminate = 1;

		for (i = 0; i < _NUM_AUDIO_CHANNELS; i++)
		{
			if (AudioStatus[i].threadHandle != -1)
			{
				sceKernelDeleteThread(AudioStatus[i].threadHandle);
			}

			AudioStatus[i].threadHandle = -1;
		}

		AudioReady = 0;
		return(1);
	}
	return(1);
}

void AudioShutdown()
{
	int i;
	AudioReady = 0;
	AudioTerminate = 1;

	for (i = 0; i < _NUM_AUDIO_CHANNELS; i++)
	{
		if (AudioStatus[i].threadHandle != -1)
		{
			sceKernelWaitThreadEnd(AudioStatus[i].threadHandle,0);
			sceKernelDeleteThread(AudioStatus[i].threadHandle);
		}

		AudioStatus[i].threadHandle = -1;
	}

	for (i = 0; i < _NUM_AUDIO_CHANNELS; i++)
	{
		if (AudioStatus[i].handle != -1)
		{
			sceAudioChRelease(AudioStatus[i].handle);
			AudioStatus[i].handle = -1;
		}
	}
}
