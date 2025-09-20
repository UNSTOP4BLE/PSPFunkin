//done
#include <cstdio>
#include <cstring>
#include <algorithm>
#include <cassert>
#include <vorbis/vorbisfile.h>
#include "audio_file_readers.hpp"

// Private stuff

#define _blocksToSamples(x) (((x) / 16) * 28)
#define _samplesToBlocks(x) (((x) / 28) * 16)

constexpr static inline uint32_t operator"" _m(const char *const str, size_t length) {
    (void)length;
    return str[0] | (str[1] << 8) | (str[2] << 16) | (str[3] << 24);
}

static inline uint32_t _swapEndian(uint32_t value) {
    value = ((value & 0xffff0000) >> 16) | ((value & 0x0000ffff) << 16);
    value = ((value & 0xff00ff00) >>  8) | ((value & 0x00ff00ff) <<  8);
    return value;
}

// THIS WILL BREAK ON BIG ENDIAN PLATFORMS (don't remove this warning)
template<typename T> static inline T _readValue(FILE *fp) {
    T value;
    assert(fread(&value, sizeof(T), 1, fp));
    return value;
}

template<typename T> static inline void _readValueInPlace(FILE *fp, T &value) {
    assert(fread(&value, sizeof(T), 1, fp));
}

namespace AUDIO {

WAVFileReader::WAVFileReader(const std::string& path) {
    _wavFile = fopen(path.c_str(), "rb");
    assert(_wavFile);

    assert(_readValue<uint32_t>(_wavFile) == "RIFF"_m);
    _readValue<uint32_t>(_wavFile); //ignore total file size
    assert(_readValue<uint32_t>(_wavFile) == "WAVE"_m);

    while (!feof(_wavFile)) {
        auto chunkType = _readValue<uint32_t>(_wavFile);
        auto chunkLength = _readValue<uint32_t>(_wavFile);

        switch (chunkType) {
            case "fmt "_m:
                WAVFormatChunk fmt;
                _readValueInPlace(_wavFile, fmt);
                // 1 is int PCM, 3 is float PCM, 0x11 is ADPCM
                assert(fmt.format == 1);

                channels = fmt.channels;
                sampleRate = fmt.sampleRate;
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

    fclose(_wavFile);
    assert(false); //wav has missing chunks
}

int WAVFileReader::getPosition(void) {
    return (ftell(_wavFile) - _dataOffset) / bytesPerSample;
}

int WAVFileReader::setPosition(int sampleOffset) {
    fseek(_wavFile, _dataOffset + (sampleOffset * bytesPerSample), SEEK_SET);

    return getPosition();
}

int WAVFileReader::read(AudioBuffer &buf, int numSamples, int bufferOffset) {
    int actualNumSamples = std::min(numSamples, totalNumSamples - getPosition());
    if (!actualNumSamples)
        return 0;

    buf.data.resize((actualNumSamples + bufferOffset) * bytesPerSample);
    buf.channels = channels;
    buf.sampleRate = sampleRate;
    buf.format = format;

    auto ptr = buf.data.data() + (bufferOffset * bytesPerSample);
    return fread(ptr, bytesPerSample, actualNumSamples, _wavFile);
}

WAVFileReader::~WAVFileReader(void) {
    fclose(_wavFile);
}

OGGFileReader::OGGFileReader(const std::string& path) {
    int error = ov_fopen(path.c_str(), &_oggFile);
    assert(!error);

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
    assert(offset != OV_EINVAL);

    return static_cast<int>(offset);
}

int OGGFileReader::setPosition(int sampleOffset) {
    int error = ov_pcm_seek(&_oggFile, sampleOffset);
    assert(!error);

    return getPosition();
}

int OGGFileReader::read(AudioBuffer &buf, int numSamples, int bufferOffset) {
    buf.data.resize((numSamples + bufferOffset) * bytesPerSample);
    buf.channels = channels;
    buf.sampleRate = sampleRate;
    buf.format = format;

    auto ptr = reinterpret_cast<char *>(
        buf.data.data() + (bufferOffset * bytesPerSample)
    );

    for (int actualNumSamples = 0; actualNumSamples < numSamples;) {
        int remaining = (numSamples - actualNumSamples) * bytesPerSample;
        int length = ov_read(&_oggFile, ptr, remaining, false, sizeof(int16_t), true, &_bitstreamIndex);
        assert(length >= 0);

        if (!length) { // End of file
            buf.data.resize((actualNumSamples + bufferOffset) * bytesPerSample);
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

FileReader *openFile(const std::string& path) {
    const char *ext = &path[path.size() - 4];

    if (!strcmp(ext, ".wav"))
        return (FileReader *) new WAVFileReader(path);
    if (!strcmp(ext, ".ogg"))
        return (FileReader *) new OGGFileReader(path);

    assert(false); // unsupported audio file format
    return nullptr;
}

bool loadFile(AudioBuffer &buffer, const std::string& path) {
    auto reader = openFile(path);
    if (!reader)
        return false;

    reader->read(buffer, reader->totalNumSamples);
    delete reader;
    return true;
}

}
