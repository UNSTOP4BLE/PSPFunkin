
#include <cstdio>
#include <cstring>
#include <algorithm>
#include <vorbis/vorbisfile.h>
#include "../main.h"
#include "audio_file_readers.h"

// Private stuff

#define _blocksToSamples(x) (((x) / 16) * 28)
#define _samplesToBlocks(x) (((x) / 28) * 16)

constexpr static inline uint32_t operator"" _m(const char *const str, size_t length) {
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
    ASSERTFUNC(fread(&value, sizeof(T), 1, fp), "file read failure");
    return value;
}

template<typename T> static inline void _readValueInPlace(FILE *fp, T &value) {
    ASSERTFUNC(fread(&value, sizeof(T), 1, fp), "file read failure");
}

namespace Audio {

WAVFileReader::WAVFileReader(const char *path) {
    _wavFile = fopen(path, "rb");
    ASSERTFUNC(_wavFile, "failed to open WAV file");

    ASSERTFUNC(_readValue<uint32_t>(_wavFile) == "RIFF"_m, "not a valid WAV file");
    _readValue<uint32_t>(_wavFile); //ignore total file size
    ASSERTFUNC(_readValue<uint32_t>(_wavFile) == "WAVE"_m, "not a valid WAV file");

    while (!feof(_wavFile)) {
        auto chunkType = _readValue<uint32_t>(_wavFile);
        auto chunkLength = _readValue<uint32_t>(_wavFile);

        switch (chunkType) {
            case "fmt "_m:
                WAVFormatChunk fmt;
                _readValueInPlace(_wavFile, fmt);
                // 1 is int PCM, 3 is float PCM, 0x11 is ADPCM
                ASSERTFUNC(fmt.format == 1, "WAV file is not PCM");

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
    ASSERTFUNC(false, "WAV file has missing chunks");
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

OGGFileReader::OGGFileReader(const char *path) {
    int error = ov_fopen(path, &_oggFile);
    ASSERTFUNC(!error, "failed to open OGG file");

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
    ASSERTFUNC(offset != OV_EINVAL, "OGG seek error");

    return static_cast<int>(offset);
}

int OGGFileReader::setPosition(int sampleOffset) {
    int error = ov_pcm_seek(&_oggFile, sampleOffset);
    ASSERTFUNC(!error, "OGG seek error");

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
        ASSERTFUNC(length >= 0, "OGG decode error");

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

VAGFileReader::VAGFileReader(const char *path) {
    _vagFile = fopen(path, "rb");
    ASSERTFUNC(_vagFile, "failed to open VAG file");

    VAGHeader header;
    _readValueInPlace(_vagFile, header);

    switch (header.magic) {
        case "VAGp"_m: // Mono
            channels = 1;
            bytesPerSample = sizeof(int16_t);
            _interleave = 28;

            _dataOffset = sizeof(VAGHeader);
            break;

        case "VAGi"_m: // Interleaved
            channels = header.channels ? header.channels : 1;
            bytesPerSample = sizeof(int16_t) * channels;
            _interleave = _blocksToSamples(header.interleave);

            _dataOffset = 2048; // VAGi header is always padded to 2048 bytes
            fseek(_vagFile, 2048, SEEK_SET);
            break;

        default:
            fclose(_vagFile);
            ASSERTFUNC(false, "not a valid VAG file");
    }

    format = AUDIO_S16SYS;
    sampleRate = _swapEndian(header.sampleRate);
    totalNumSamples = _blocksToSamples(_swapEndian(header.size));

    _bufferPosition = 0;
    _sampleBufferLength = 0;
    _chunkBuffer = new uint8_t[_interleave * channels];
    _sampleBuffer = new int16_t[_blocksToSamples(_interleave) * channels];
}

int VAGFileReader::getPosition(void) {
    return (_blocksToSamples(ftell(_vagFile) - _dataOffset) / channels) + _bufferPosition;
}

int VAGFileReader::setPosition(int sampleOffset) {
    _bufferPosition = sampleOffset % _interleave;
    _sampleBufferLength = 0; // Force chunk to be reloaded

    int chunkOffset = sampleOffset - _bufferPosition;
    fseek(_vagFile, _dataOffset + _samplesToBlocks(chunkOffset) * channels, SEEK_SET);
    _decoder.reset();

    return getPosition();
}

int VAGFileReader::read(AudioBuffer &buf, int numSamples, int bufferOffset) {
    buf.data.resize((numSamples + bufferOffset) * bytesPerSample);
    buf.channels = channels;
    buf.sampleRate = sampleRate;
    buf.format = format;

    int interleaveSize = _samplesToBlocks(_interleave);
    auto ptr = buf.data.data() + (bufferOffset * bytesPerSample);

    for (int actualNumSamples = 0; actualNumSamples < numSamples;) {
        //int remaining = std::min(numSamples - actualNumSamples, _interleave);

        if (!_sampleBufferLength) {
            int length = fread(_chunkBuffer, 1, interleaveSize * channels, _vagFile);
            ASSERTFUNC(length >= 0, "VAG read error");

            if (!length) { // End of file
                buf.data.resize((actualNumSamples + bufferOffset) * bytesPerSample);
                return actualNumSamples;
            }

            // Decode all channels and interleave them in the sample buffer
            for (int ch = 0; ch < channels; ch++)
                _sampleBufferLength = _decoder.decode(
                    &_sampleBuffer[ch],
                    &_chunkBuffer[interleaveSize * ch],
                    interleaveSize,
                    channels
                );
        }

        int length = _sampleBufferLength * bytesPerSample;
        memcpy(ptr, &_sampleBuffer[_bufferPosition], length);

        actualNumSamples += _sampleBufferLength;
        ptr += _sampleBufferLength * bytesPerSample;
        _bufferPosition += _sampleBufferLength;

        if (_bufferPosition >= _interleave) {
            // If the full chunk has been read, schedule the next chunk for decoding
            _bufferPosition -= _interleave;
            _sampleBufferLength = 0;
        }
    }

    return numSamples;
}

VAGFileReader::~VAGFileReader(void) {
    fclose(_vagFile);

    delete[] _chunkBuffer;
    delete[] _sampleBuffer;
}

FileReader *openFile(const char *path) {
    const char *ext = &path[strlen(path) - 4];

    if (!strcmp(ext, ".wav"))
        return (FileReader *) new WAVFileReader(path);
    if (!strcmp(ext, ".ogg"))
        return (FileReader *) new OGGFileReader(path);
    // Sony engineer: "I swear it doesn't stand for what you think it stands for..."
    if (!strcmp(ext, ".vag") || !strcmp(ext, ".vagi"))
        return (FileReader *) new VAGFileReader(path);

    ASSERTFUNC(false, "unsupported audio file format");
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
