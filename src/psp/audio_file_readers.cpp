
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
    ASSERTFUNC(fread(&value, sizeof(T), 1, fp));
    return value;
}

template<typename T> static inline void _readValueInPlace(FILE *fp, T &value) {
    ASSERTFUNC(fread(&value, sizeof(T), 1, fp));
}

namespace Audio {

WAVFileReader::WAVFileReader(const char *path) {
    wavFile = fopen(path, "rb");
    ASSERTFUNC(wavFile);

    ASSERTFUNC(_readValue<uint32_t>(wavFile) == "RIFF"_m);
    _readValue<uint32_t>(wavFile); //ignore total file size
    ASSERTFUNC(_readValue<uint32_t>(wavFile) == "WAVE"_m);

    while (!feof(wavFile)) {
        uint32_t chunkType = _readValue<uint32_t>(wavFile);
        uint32_t chunkLength = _readValue<uint32_t>(wavFile);

        if (chunkType == "fmt "_m) {
            _readValueInPlace(wavFile, fmt);
            ASSERTFUNC(fmt.format == 1); // 1 is int PCM, 3 is float PCM, 0x11 is ADPCM

            // ugly hack but SDL doesn't provide any API for this
            format = fmt.bps & SDL_AUDIO_MASK_BITSIZE;
            if (fmt.bps >= 16) format |= SDL_AUDIO_MASK_SIGNED;

            bytesPerSample = fmt.channels * fmt.bps / 8;
        } else if (chunkType == "data"_m) {
            dataOffset = ftell(wavFile);
            totalNumSamples = chunkLength / bytesPerSample;
            return;
        } else {
            // skip the chunk
            fseek(wavFile, chunkLength, SEEK_CUR);
        }
    }

    ASSERTFUNCFAIL("failed to parse file");
    fclose(wavFile);
}

int WAVFileReader::getPosition(void) {
    return (ftell(wavFile) - dataOffset) / bytesPerSample;
}

int WAVFileReader::setPosition(int sampleOffset) {
    fseek(wavFile, dataOffset + (sampleOffset * bytesPerSample), SEEK_SET);

    return getPosition();
}

int WAVFileReader::read(AudioBuffer &buf, int numSamples) {
    int actualNumSamples = std::min(numSamples, int(totalNumSamples - getPosition()));
    if (!actualNumSamples) return 0;

    buf.data.resize(actualNumSamples * bytesPerSample);
    buf.channels = fmt.channels;
    buf.samplerate = fmt.samplerate;
    buf.format = format;

    return fread(buf.data.data(), bytesPerSample, actualNumSamples, wavFile);
}

int WAVFileReader::getLength(void) {
    return totalNumSamples;
}

WAVFileReader::~WAVFileReader(void) {
    fclose(wavFile);
}

OGGFileReader::OGGFileReader(const char *path) {
    int error = ov_fopen(path, &oggFile);
    ASSERTFUNC(!error);

    info = ov_info(&oggFile, -1);
    totalNumSamples = ov_pcm_total(&oggFile, -1);
    bytesPerSample = sizeof(int16_t) * info->channels;
    bitstreamIndex = 0;
}

int OGGFileReader::getPosition(void) {
    auto offset = ov_pcm_tell(&oggFile);
    ASSERTFUNC(offset != OV_EINVAL);

    return static_cast<int>(offset);
}

int OGGFileReader::setPosition(int sampleOffset) {
    int error = ov_pcm_seek(&oggFile, sampleOffset);
    ASSERTFUNC(!error);

    return getPosition();
}

int OGGFileReader::read(AudioBuffer &buf, int numSamples) {
    buf.data.resize(numSamples * bytesPerSample);
    buf.channels = info->channels;
    buf.samplerate = info->rate;
    buf.format = AUDIO_S16LSB;

    auto ptr = reinterpret_cast<char *>(buf.data.data());

    for (int actualNumSamples = 0; actualNumSamples < numSamples;) {
        int remaining = (numSamples - actualNumSamples) * bytesPerSample;
        int length = ov_read(&oggFile, ptr, remaining, false, sizeof(int16_t), true, &bitstreamIndex);
        ASSERTFUNC(length >= 0);

        if (!length) { // end of file
            buf.data.resize(actualNumSamples * bytesPerSample);
            return actualNumSamples;
        }

        actualNumSamples += length / bytesPerSample;
        ptr += length;
    }

    return numSamples;
}

int OGGFileReader::getLength(void) {
    return totalNumSamples;
}

OGGFileReader::~OGGFileReader(void) {
    ov_clear(&oggFile);
}

FileReader *openFile(const char *path) {
    const char *ext = &path[strlen(path) - 4];

    if (!strcmp(ext, ".wav"))
        return (FileReader *) new WAVFileReader(path);
    if (!strcmp(ext, ".ogg"))
        return (FileReader *) new OGGFileReader(path);

    ASSERTFUNCFAIL("unsupported audio format");
    return nullptr;
}

AudioBuffer *loadFile(const char *path) {
    auto reader = openFile(path);
    if (!reader)
        return nullptr;

    auto buffer = new AudioBuffer();
    reader->read(*buffer, reader->getLength());
    delete reader;

    return buffer;
}

}
