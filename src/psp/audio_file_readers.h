#pragma once

#include <cstdint>
#include <vorbis/vorbisfile.h>
#include "../main.h"
#include "audio_buffer.h"

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
    int channels, sampleRate, bytesPerSample, totalNumSamples;
    SDL_AudioFormat format;

    virtual int getPosition(void) { return 0; }
    virtual int setPosition(int sampleOffset) { return 0; }
    virtual int read(AudioBuffer &buf, int numSamples, int bufferOffset = 0) { return 0; }
    virtual ~FileReader(void) {}
};

class WAVFileReader : public FileReader {
private:
    FILE *_wavFile;
    int _dataOffset;

public:
    WAVFileReader(const char *path);
    int getPosition(void);
    int setPosition(int sampleOffset);
    int read(AudioBuffer &buf, int numSamples, int bufferOffset = 0);
    ~WAVFileReader(void);
};

class OGGFileReader : public FileReader {
private:
    OggVorbis_File _oggFile;
    int _bitstreamIndex;

public:
    OGGFileReader(const char *path);
    int getPosition(void);
    int setPosition(int sampleOffset);
    int read(AudioBuffer &buf, int numSamples, int bufferOffset = 0);
    ~OGGFileReader(void);
};

FileReader *openFile(const char *path);
AudioBuffer *loadFile(const char *path);

}
