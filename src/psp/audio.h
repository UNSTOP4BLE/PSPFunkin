#ifndef __AUDIO_H__
#define __AUDIO_H__

#define _NUM_AUDIO_CHANNELS	4
#define _NUM_AUDIO_SAMPLES	1024
#define _VOLUME_MAX		0x8000

typedef void (* AudioCallback)(void *buf, int reqn, void *pdata);

typedef struct
{
	int threadHandle;
        int handle;
        int volumeLeft;
        int volumeRight;
        AudioCallback callback;
        void *data;
} AudioChannelInfo;

bool AudioInit(int priority); // 0x12
void AudioShutdown();
void AudioSetVolume(int channel, int left, int right);
void AudioSetChannelCallback(int channel, AudioCallback callback, void *data);
int AudioOutBlocking(int channel, int left, int right, void *data);

#endif
