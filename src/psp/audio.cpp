#include "../main.h"
#include "audio.h"
#include <SDL2/SDL.h>                                                                                                                                                                                
#include "../screen.h"

constexpr static inline uint32_t operator"" _m(const char *const str, size_t length) {
  return str[0] | (str[1] << 8) | (str[2] << 16) | (str[3] << 24);
}

// THIS WILL BREAK ON BIG ENDIAN PLATFORMS (don't remove this warning)
template<typename T> static inline T _readValue(FILE *fp) {
    T value;
    ASSERTFUNC(fread(&value, sizeof(T), 1, fp));   
    return value;
}

template<typename T> static inline void _readValueInPlace(FILE *fp, T &value) {
    ASSERTFUNC(fread(&value, sizeof(T), 1, fp));   
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

class StreamSource {
public:
    virtual int getPosition(void);
    virtual int readBuf(AudioBuffer &buf, int numSamples); 
    virtual int getNumSamples(void);
    virtual ~StreamSource(void) {};
};

class WAVStreamSource : public StreamSource 
{
public:
    WAVStreamSource(const char *path);
    int getPosition(void);
    int readBuf(AudioBuffer &buf, int numSamples);
    int getNumSamples(void);
    ~WAVStreamSource(void);
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

    ASSERTFUNC(wavFile);   

    ASSERTFUNC(_readValue<uint32_t>(wavFile) == "RIFF"_m);
    _readValue<uint32_t>(wavFile); //ignore total file size
    ASSERTFUNC(_readValue<uint32_t>(wavFile) == "WAVE"_m);

    uint32_t chunkType = _readValue<uint32_t>(wavFile);
    uint32_t chunkLength = _readValue<uint32_t>(wavFile);
    while (!feof(wavFile)) {
        if (chunkType == "fmt "_m) {
            _readValueInPlace(wavFile, fmt);
            ASSERTFUNC(fmt.format == 1); // 1 is int PCM, 3 is float PCM, 0x11 is ADPCM

            // ugly hack but SDL doesn't provide any API for this
            format = fmt.bps;
            if (fmt.bps >= 16) format |= SDL_AUDIO_MASK_SIGNED;
        } else if (chunkType == "data"_m) {
            dataOffset = ftell(wavFile);    
            totalNumSamples = chunkLength / (fmt.channels * SDL_AUDIO_BITSIZE(format) / 8);
            return;
        } else {
            // skip the chunk
            fseek(wavFile, chunkLength, SEEK_CUR);
        }
    }
    ASSERTFUNC(false);  //fail to read file
    fclose(wavFile);
}

int WAVStreamSource::getPosition(void){ 
    return (ftell(wavFile) - dataOffset) / (fmt.channels * SDL_AUDIO_BITSIZE(format) / 8);
}

int WAVStreamSource::readBuf(AudioBuffer &buf, int numSamples) {
    int actualNumSamples = std::min(numSamples, (int) (totalNumSamples - getPosition()));
    if (!actualNumSamples) return 0;

    buf.data.resize(actualNumSamples * fmt.channels * SDL_AUDIO_BITSIZE(format) / 8);
    buf.channels = fmt.channels;
    buf.samplerate = fmt.samplerate;

    return fread(buf.data.data(), fmt.channels * SDL_AUDIO_BITSIZE(format) / 8, actualNumSamples, wavFile);
}

int WAVStreamSource::getNumSamples(void) {
    return totalNumSamples;
}

WAVStreamSource::~WAVStreamSource(void) {
    fclose(wavFile);
}

void Audio_Init(void)
{
    audioMixer = new Mixer(44100); //hardcoded sample rate sucks but eh
    audioMixer->setMasterVolume(100);
}

AudioBuffer *Audio_LoadFile(const char *path) {
    const char *ext = &path[strlen(path) - 4];
    
    StreamSource *source = NULL;
    ASSERTFUNC(strcmp(ext, ".wav") || strcmp(ext, ".ogg"));
    if (!strcmp(ext, ".wav"))
        source = new WAVStreamSource(path);

    //else if (!strcmp(ext, ".ogg"))
   //     source = new OGGStreamSource(path);
    else
        ASSERTFUNC(false);

    AudioBuffer *buffer = new AudioBuffer();
    source->readBuf(*buffer, source->getNumSamples());
    delete source;
    return buffer;
}