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
	uint16_t allign;
	uint16_t bps;
};

class WAVStreamSource
{
public:
	inline WAVStreamSource(const char *path) {
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
		//	ErrMSG("INVALID WAV FILE");
		//	close();
	//		return;
	//	}  
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
				dataLength = chunkLength;
				return;
			}  
			else
				fseek(wavFile, chunkLength, SEEK_CUR);
		}
	}

	inline ~WAVStreamSource(void) {
		fclose(wavFile);
	}
private:
	FILE *wavFile;
	FMT fmt;
	int dataOffset;
	uint32_t dataLength;
};

class AudioBuffer {
public:
	uint16_t *data;
	uint16_t channels;
	uint32_t samplerate;
	uint32_t dataLength;
private:


};