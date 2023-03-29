#include <cstdio>
#include <cstring>
#include <vorbis/vorbisfile.h>
#include "../main.h"
#include "audio_file_readers.h"

// Private stuff

constexpr static inline uint32_t operator"" _m(const char *const str, size_t length) {
  return str[0] | (str[1] << 8) | (str[2] << 16) | (str[3] << 24);
}

// THIS WILL BREAK ON BIG ENDIAN PLATFORMS (don't remove this warning)
template<typename T> static inline T _readValue(FILE *fp) {
    T value;
    ASSERTFUNC(fread(&value, sizeof(T), 1, fp), "fail to read value");
    return value;
}

template<typename T> static inline void _readValueInPlace(FILE *fp, T &value) {
    ASSERTFUNC(fread(&value, sizeof(T), 1, fp), "fail to read value");
}

namespace Audio {

WAVFileReader::WAVFileReader(const char *path) {
    _wavFile = fopen(path, "rb");
    ASSERTFUNC(_wavFile, "failed to open wav file");

    ASSERTFUNC(_readValue<uint32_t>(_wavFile) == "RIFF"_m, "invalid wav file");
    _readValue<uint32_t>(_wavFile); //ignore total file size
    ASSERTFUNC(_readValue<uint32_t>(_wavFile) == "WAVE"_m, "invalid wav file");

    while (!feof(_wavFile)) {
        uint32_t chunkType = _readValue<uint32_t>(_wavFile);
        uint32_t chunkLength = _readValue<uint32_t>(_wavFile);

        switch (chunkType) {
            case "fmt "_m:
                WAVFormatChunk fmt;
                _readValueInPlace(_wavFile, fmt);
                ASSERTFUNC(fmt.format == 1, "invalid wav file"); // 1 is int PCM, 3 is float PCM, 0x11 is ADPCM

                channels = fmt.channels;
                sampleRate = fmt.samplerate;
                bytesPerSample = fmt.channels * fmt.bps / 8;

                format = fmt.bps & SDL_AUDIO_MASK_BITSIZE;
                if (fmt.bps >= 16) format |= SDL_AUDIO_MASK_SIGNED;
                break;

            case "data"_m:
                _dataOffset = ftell(_wavFile);
                totalNumSamples = chunkLength / bytesPerSample;
                return;

            default:
                // skip the chunk
                fseek(_wavFile, chunkLength, SEEK_CUR);
        }
    }

    ASSERTFUNC(false, "failed to parse file");
    fclose(_wavFile);
}

int WAVFileReader::getPosition(void) {
    return (ftell(_wavFile) - _dataOffset) / bytesPerSample;
}

int WAVFileReader::setPosition(int sampleOffset) {
    fseek(_wavFile, _dataOffset + (sampleOffset * bytesPerSample), SEEK_SET);

    return getPosition();
}

int WAVFileReader::read(AudioBuffer &buf, int numSamples) {
    int actualNumSamples = std::min(numSamples, totalNumSamples - getPosition());
    if (!actualNumSamples)
        return 0;

    buf.data.resize(actualNumSamples * bytesPerSample);
    buf.channels = channels;
    buf.sampleRate = sampleRate;
    buf.format = format;

    return fread(buf.data.data(), bytesPerSample, actualNumSamples, _wavFile);
}

WAVFileReader::~WAVFileReader(void) {
    fclose(_wavFile);
}

OGGFileReader::OGGFileReader(const char *path) {
    int error = ov_fopen(path, &_oggFile);
    ASSERTFUNC(!error, "failed to open ogg file");

    auto info = ov_info(&_oggFile, -1);
    totalNumSamples = ov_pcm_total(&_oggFile, -1);

    format = AUDIO_S16LSB;
    channels = info->channels;
    sampleRate = info->rate;
    bytesPerSample = sizeof(int16_t) * info->channels;

    _bitstreamIndex = 0;
}

int OGGFileReader::getPosition(void) {
    auto offset = ov_pcm_tell(&_oggFile);
    ASSERTFUNC(offset != OV_EINVAL, "invalid value");

    return static_cast<int>(offset);
}

int OGGFileReader::setPosition(int sampleOffset) {
    int error = ov_pcm_seek(&_oggFile, sampleOffset);
    ASSERTFUNC(!error, "failed to seek");

    return getPosition();
}

int OGGFileReader::read(AudioBuffer &buf, int numSamples) {
    buf.data.resize(numSamples * bytesPerSample);
    buf.channels = channels;
    buf.sampleRate = sampleRate;
    buf.format = format;

    auto ptr = reinterpret_cast<char *>(buf.data.data());

    for (int actualNumSamples = 0; actualNumSamples < numSamples;) {
        int remaining = (numSamples - actualNumSamples) * bytesPerSample;
        int length = ov_read(&_oggFile, ptr, remaining, false, sizeof(int16_t), true, &_bitstreamIndex);
        ASSERTFUNC(length >= 0, "read failed");

        if (!length) { // end of file
            buf.data.resize(actualNumSamples * bytesPerSample);
            return actualNumSamples;
        }

        actualNumSamples += length / bytesPerSample;
        ptr += length;
    }

    return numSamples;
}

OGGFileReader::~OGGFileReader(void) {
    ov_clear(&_oggFile);
}

FileReader *openFile(const char *path) {
    const char *ext = &path[strlen(path) - 4];

    if (!strcmp(ext, ".wav"))
        return (FileReader *) new WAVFileReader(path);
    if (!strcmp(ext, ".ogg"))
        return (FileReader *) new OGGFileReader(path);

    ASSERTFUNC(false, "unsupported audio format");
    return nullptr;
}

AudioBuffer *loadFile(const char *path) {
    auto reader = openFile(path);
    if (!reader)
        return nullptr;

    auto buffer = new AudioBuffer();
    reader->read(*buffer, reader->totalNumSamples);
    delete reader;

    return buffer;
}

}
