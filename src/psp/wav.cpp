#include <pspaudio.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include "wav.h"
#include "audio.h"

#define SND_MAXSLOT 16

static Wav wavout_snd_wavinfo[SND_MAXSLOT];
static int wavout_snd_playing[SND_MAXSLOT];
static int wavout_snd_id[SND_MAXSLOT];

static signed short *samples;
static unsigned long req;
static int id = 0;

typedef struct Refcount
{
	char*	        id;
	void*	        data;
	signed long		refs;
	
	struct Refcount* next;
} Refcount;


static Refcount* RefcountList = 0;


void RefcountCreate( const char* id, void* data )
{
	if (id==0 || data==0) return;
	Refcount* r = malloc(sizeof(Refcount));
	if (r==0) return;
	r->id = malloc(strlen(id)+1);
	strcpy(r->id, id);
	r->id[strlen(id)] = 0;
	r->data = data;
	r->refs = 1;
	r->next = RefcountList;
	RefcountList = r;
}

void* RefcountRetain( const char* id )
{
	if (id==0) return(0);
	Refcount* r = RefcountList;
	
	while (r!=0)
	{
		if (strcmp(id, r->id)==0)
		{
			r->refs++;
			return(r->data);
		}
		r = r->next;
	}
	return(0);
}

signed long RefcountRelease( const void* data )
{
	if (data==0) return -1;
	Refcount* r = RefcountList;
	Refcount* l = 0;
	
	while (r!=0)
	{
		if (data == r->data)
		{
			r->refs--;
			if (r->refs==0)
			{
				if (l==0)
					RefcountList = r->next;
				else
					l->next = r->next;
				free(r->id);
				free(r);
				return(0);
			}
			return(r->refs);
		}
		l = r;
		r = r->next;
	}
	return(0);
}

static void wavout_snd_callback(void *_buf, int _reqn, void *pdata)
{
	int i,slot;
	Wav *wi;
	unsigned long ptr, frac;
	signed short *buf = _buf;
	
	samples = _buf;
	req = _reqn;
	
	for (i = 0; i < _reqn; i++)
	{
		int outr = 0, outl = 0;
		
		for (slot = 0; slot < SND_MAXSLOT; slot++)
		{
			if (!wavout_snd_playing[slot]) continue;
			
			wi = &wavout_snd_wavinfo[slot];
			frac = wi->playPtr_frac + wi->rateRatio;
			wi->playPtr = ptr = wi->playPtr + (frac>>16);
			wi->playPtr_frac = (frac & 0xffff);

			if (ptr >= wi->sampleCount)
			{
				if (wi->loop)
				{
					wi->playPtr = 0;
					wi->playPtr_frac = 0;
					ptr = 0;
				}
				else
				{
					wavout_snd_playing[slot] = 0;
					break;
				}
			}
			
			signed short *src16 = (signed short *)wi->data;
			unsigned char *src8 = (unsigned char *)wi->data;

			if (wi->channels == 1)
			{
				if(wi->bitPerSample == 8)
				{
					outl += (src8[ptr] * 256) - 32768;
					outr += (src8[ptr] * 256) - 32768;
				}
				else
				{
					outl += src16[ptr];
					outr += src16[ptr];
				}
			}
			else
			{
				if(wi->bitPerSample == 8)
				{
					outl += (src8[ptr*2] * 256) - 32768;
					outr += (src8[ptr*2+1] * 256) - 32768;
				}
				else
				{
					outl += src16[ptr*2];
					outr += src16[ptr*2+1];
				}
			}
		}
		
		if (outl < -32768)
			outl = -32768;
		else if (outl > 32767)
			outl = 32767;
			
		if (outr < -32768)
			outr = -32768;
		else if (outr > 32767)
			outr = 32767;

		*(buf++) = outl;
		*(buf++) = outr;
	}
}

bool Wav_Init()
{
	int i;

	AudioInit(0x12);

	AudioSetChannelCallback(0, wavout_snd_callback, 0);

	for (i = 0; i < SND_MAXSLOT; i++)
	{
		wavout_snd_playing[i] = 0;
	}

	return(1);
}

void Wav_Stop(Wav *theWav)
{
	int i;
	
	for (i = 0; i < SND_MAXSLOT; i++)
	{
		if(theWav->id == wavout_snd_id[i])
			wavout_snd_playing[i] = 0;
	}
}

void Wav_StopAll()
{
	int i;
	
	for (i = 0; i < SND_MAXSLOT; i++)
		wavout_snd_playing[i] = 0;
}

void Wav_SetLoop(Wav *theWav, int loop)
{
	theWav->loop = loop;
}

bool Wav_Play(Wav *theWav)
{
	int i;
	Wav *wid;

	for ( i = 0; i < SND_MAXSLOT; i++ ) if ( wavout_snd_playing[i] == 0 ) break;
	
	if ( i == SND_MAXSLOT ) return(0);
	
	wid = &wavout_snd_wavinfo[i];
	wid->channels = theWav->channels;
	wid->sampleRate = theWav->sampleRate;
	wid->sampleCount = theWav->sampleCount;
	wid->dataLength = theWav->dataLength;
	wid->data = theWav->data;
	wid->rateRatio = theWav->rateRatio;
	wid->playPtr = 0;
	wid->playPtr_frac = 0;
	wid->loop = theWav->loop;
	wid->id = theWav->id;
	wavout_snd_playing[i] = 1;
	wavout_snd_id[i] = theWav->id;
	wid->bitPerSample = theWav->bitPerSample;

	return(1);
}


inline signed short readS16(char* file, unsigned long addr )
{
	return (signed short)((file[addr]) | (file[addr+1]<<8));
}

inline unsigned short readU16(char* file, unsigned long addr )
{
	return (unsigned short)((file[addr]) | (file[addr+1]<<8));
}

inline unsigned long readS32(char* file, unsigned long addr )
{
	return (unsigned long)((file[addr]) | (file[addr+1]<<8) | (file[addr+2]<<16) | (file[addr+3]<<24));
}

inline unsigned long readU32(char* file, unsigned long addr )
{
	return (unsigned long)((file[addr]) | (file[addr+1]<<8) | (file[addr+2]<<16) | (file[addr+3]<<24));
}

Wav *Wav_Load(const char *filename)
{
	int filelen;
	unsigned long channels;
	unsigned long samplerate;
	unsigned long bitpersample;
	unsigned long datalength;
	unsigned long samplecount;
	char *wavfile;
	Wav *theWav;
	
	if ((theWav=RefcountRetain( filename ))!=0)
		return theWav;
	
	FILE *pFile = fopen(filename , "rb");
	
	if(pFile == NULL)
	{
		return NULL;
	}
	
	unsigned long lSize;

	fseek(pFile , 0, SEEK_END);
	lSize = ftell(pFile);
	rewind(pFile);
	
	theWav = malloc(lSize + sizeof(Wav));
	RefcountCreate( filename, theWav );
	wavfile = (char*)(theWav) + sizeof(Wav);
	
	filelen = fread(wavfile, 1, lSize, pFile);
	
	fclose(pFile);
	
	if (memcmp(wavfile, "RIFF", 4) != 0)
	{
		free(theWav);
		return NULL;
	}
	
	channels = *(signed short *)(wavfile + 0x16);
	samplerate = *(unsigned long *)(wavfile + 0x18);
	bitpersample = *(signed short *)(wavfile + 0x22);

	int i;
	
	for(i = 0; memcmp(wavfile + 0x24 + i, "data", 4) != 0; i++)
	{
		if (i == filelen-0x2c)
		{
			free(theWav);
			return NULL;
		}
	}

	datalength = readU32(wavfile, 0x28+i); 
	
	if (datalength + 0x2c + i > filelen)
	{
		datalength = filelen - i - 0x2c;
	}
	
	if (channels != 2 && channels != 1)
	{
		free(theWav);
		return NULL;
	}
	
	if (samplerate > 100000 || samplerate < 2000)
	{
		free(theWav);
		return NULL;
	}

	if (channels == 2)
	{
		samplecount = datalength/(bitpersample/4);
	}
	else
	{
		samplecount = datalength/((bitpersample/4)/2);
	}
	
	if (samplecount <= 0)
	{
		free(theWav);
		return NULL;
	}
	
	theWav->channels = channels;
	theWav->sampleRate = samplerate;
	theWav->sampleCount = samplecount;
	theWav->dataLength = datalength;
	theWav->data = wavfile + 0x2c + i;
	theWav->rateRatio = (samplerate*0x4000)/11025;
	theWav->playPtr = 0;
	theWav->playPtr_frac= 0;
	theWav->loop = 0;
	id++;
	theWav->id = id;
	theWav->bitPerSample = bitpersample;

	return theWav;
}

void Wav_free(Wav *theWav)
{
	if (RefcountRelease( theWav )!=0) return;
	if(theWav != NULL)
		free(theWav);
}

bool Wav_Playing()
{
	if ((
		wavout_snd_playing[0] ||
		wavout_snd_playing[1] ||
		wavout_snd_playing[2] ||
		wavout_snd_playing[3] ||
		wavout_snd_playing[4] ||
		wavout_snd_playing[5] ||
		wavout_snd_playing[6] ||
		wavout_snd_playing[7] || 
		wavout_snd_playing[8] ||
		wavout_snd_playing[9] || 
		wavout_snd_playing[10] ||
		wavout_snd_playing[11] ||
		wavout_snd_playing[12] ||
		wavout_snd_playing[13] || 
		wavout_snd_playing[14] ||
		wavout_snd_playing[15]) == 1
		)
		return true;
	else
		return false;
}