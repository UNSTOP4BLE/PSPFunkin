#include "audio.h"
#include <SDL2/SDL.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>
#include "../screen.h"

constexpr static inline uint32_t operator"" _m(const char *const str, size_t length) {
  return str[0] | (str[1] << 8) | (str[2] << 16) | (str[3] << 24);
}

// THIS WILL BREAK ON BIG ENDIAN PLATFORMS (don't remove this warning)
template<typename T> static inline T _readValue(FILE *fp) {
	T value;
	assert(fread(&value, sizeof(T), 1, fp));
	return T;
}

template<typename T> static inline void _readValueInPlace(FILE *fp, T &value) {
	assert(fread(&value, sizeof(T), 1, fp));
}

struct __attribute__((packed))FMT
{
	uint16_t format;
	uint16_t channels;
	uint32_t samplerate;
	uint32_t byterate;
	uint16_t align;
	uint16_t bps;
};

class AudioBuffer {
public:

    inline int getNumSamples(void) {
        return data.size() / (channels * SDL_AUDIO_BITSIZE(format) / 8);
    }
    std::vector<uint8_t> data; 
    uint16_t channels;
    uint32_t samplerate;
    SDL_AudioFormat format;
};

class WAVStreamSource
{
public:
	WAVStreamSource(const char *path);

    inline int getPosition(void)
    {
        return (ftell(wavFile) - dataOffset) / (fmt.channels * SDL_AUDIO_BITSIZE(format) / 8);
    }

    int readBuf(AudioBuffer &buf, int numSamples);

	inline ~WAVStreamSource(void) {
		fclose(wavFile);
	}
private:
	FILE *wavFile;
	FMT fmt;
	int dataOffset;
	uint32_t totalNumSamples;
    SDL_AudioFormat format;
};

WAVStreamSource::WAVStreamSource(const char *path)
{
    wavFile = fopen(path, "rb");

    if (wavFile == NULL)
    {
        ErrMSG("FAILED TO FIND WAV FILE AT PATH %s", path);
        close();
        return;
    }

    assert(_readValue<uint32_t>(wavFile) == "RIFF"_m);
    _readValue<uint32_t>(wavFile); //ignore total file size
    assert(_readValue<uint32_t>(wavFile) == "WAVE"_m);

        //if (_readValue<uint32_t>(wavFile) != ("RIFF"_m || "WAVE"_m))
        //{
        //  ErrMSG("INVALID WAV FILE");
        //  close();
    //      return;
    //  }  
    uint32_t chunkType = _readValue<uint32_t>(wavFile);
    uint32_t chunkLength = _readValue<uint32_t>(wavFile);

    while (!feof(wavFile))
    {
        if (chunkType == "fmt "_m)
        {
            _readValueInPlace(wavFile, fmt);
            assert(fmt.format == 1); // 1 is PCM, 0x11 is ADPCM
            assert(fmt.bps == 16);
        }
        else if (chunkType == "data"_m) {
            dataOffset = ftell(wavFile);    
            totalNumSamples = chunkLength / (fmt.channels * sizeof(uint16_t));
            return;
        }  
        else
            fseek(wavFile, chunkLength, SEEK_CUR);
    }
}

int WAVStreamSource::readBuf(AudioBuffer &buf, int numSamples) {
    int actualNumSamples = std::min(numSamples, totalNumSamples - getPosition());
    if (!actualNumSamples) return 0;

    buf.data.resize(actualNumSamples * fmt.channels);
    buf.channels = fmt.channels;
    buf.samplerate = fmt.samplerate;

    return fread(buf.data.data(), fmt.channels * sizeof(uint16_t), actualNumSamples, wavFile);
}