#pragma once

#include <cstdint>
#include <vorbis/vorbisfile.h>
#include "../main.h"
#include "audio_buffer.h"

namespace Audio {

struct __attribute__((packed)) WAVFormatChunk {
    uint16_t format;
    uint16_t channels;
    uint32_t sampleRate;
    uint32_t byterate;
    uint16_t align;
    uint16_t bps;
};

struct __attribute__((packed)) VAGHeader {
    uint32_t magic;         // "VAGp" or "VAGi"
    uint32_t version;
    uint32_t interleave;    // Little-endian, size of each channel buffer
    uint32_t size;          // Big-endian, in bytes (for each channel)
    uint32_t sampleRate;    // Big-endian, in Hertz
    uint16_t _reserved[5];
    uint16_t channels;      // Little-endian, if 0 the file is mono
    char     name[16];
};

class FileReader {
public:
    int channels, sampleRate, bytesPerSample, totalNumSamples;
    SDL_AudioFormat format;

    virtual int getPosition(void) { return 0; }
    virtual int setPosition(int sampleOffset) { (void) sampleOffset; return 0; }
    virtual int read(AudioBuffer &buf, int numSamples, int bufferOffset = 0) { (void) buf; (void) numSamples; (void) bufferOffset; return 0; }
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
