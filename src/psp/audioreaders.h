#pragma once
#include "../main.h"
#include "audiobuffer.h"
#include "mixer.h"
#include <cstdio>
#include <cstdint>
#include <vorbis/vorbisfile.h>
#include <vector>     

namespace Audio {

struct __attribute__((packed)) WAVFormatChunk {
    uint16_t format;
    uint16_t channels;
    uint32_t samplerate;
    uint32_t byterate;
    uint16_t align;
    uint16_t bps;
};

class FileReader {
public:
    virtual int getPosition(void);
    virtual int setPosition(int sampleOffset);
    virtual int readBuf(AudioBuffer &buf, int numSamples); 
    virtual int getLength(void);
    virtual ~FileReader(void) {}
};

class WAVFileReader : public FileReader {
private:
    FILE *wavFile;
    WAVFormatChunk fmt;
    int dataOffset, bytesPerSample;
    uint32_t totalNumSamples;
    SDL_AudioFormat format;

public:
    WAVFileReader(const char *path);
    int getPosition(void);
    int setPosition(int sampleOffset);
    int readBuf(AudioBuffer &buf, int numSamples);
    int getLength(void);
    ~WAVFileReader(void);
};

class OGGFileReader : public FileReader {
private:
    OggVorbis_File oggFile;
    vorbis_info *info;
    int bitstreamIndex, bytesPerSample;
    uint32_t totalNumSamples;

public:
    OGGFileReader(const char *path);
    int getPosition(void);
    int setPosition(int sampleOffset);
    int readBuf(AudioBuffer &buf, int numSamples);
    int getLength(void);
    ~OGGFileReader(void);
};

AudioBuffer *loadFile(const char *path);
void play(AudioBuffer *buffer);
}
