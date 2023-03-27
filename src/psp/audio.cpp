#include "../main.h"
#include "audio.h"
#include <SDL2/SDL.h>                                                                                                                                                                             
#include <cassert>    
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

// 1024-sample buffer at 44100 Hz = ~23ms latency
static constexpr int NUM_CHANNELS = 32;
static constexpr int MAX_BUFFER_SIZE = 1024;

class MixerStream {
    friend class Mixer;

private:
    SDL_AudioStream *_stream;
    int _leftVolume, _rightVolume; // fixed-point (1 << VOLUME_SHIFT)
    bool _busy;

    // These are only accessible to the mixer class.
    inline MixerStream(void)
    : _stream(nullptr), _leftVolume(INT16_MAX), _rightVolume(INT16_MAX), _busy(false) {}
    inline ~MixerStream(void) {
        if (_stream)
            SDL_FreeAudioStream(_stream);
    }

    inline void _open(SDL_AudioFormat format, int channels, int sampleRate, int mixerSampleRate) {
        if (_stream)
            SDL_FreeAudioStream(_stream);

        _stream = SDL_NewAudioStream(
            format, channels, sampleRate, // input
            AUDIO_S16SYS, 2, mixerSampleRate // output
        );
        _leftVolume = INT16_MAX;
        _rightVolume = INT16_MAX;
        _busy = true;

        assert(_stream);
    }
    inline int _read(int16_t *data, int size) {
        return SDL_AudioStreamGet(_stream, data, size);
    }

public:
    inline bool isBusy(void) {
        if (!_stream)
            return false;

        return _busy || (SDL_AudioStreamAvailable(_stream) > 0);
    }
    inline void setVolume(float left, float right) {
        _leftVolume = static_cast<int>(
            std::max(-1.0f, std::min(1.0f, left)) * static_cast<float>(INT16_MAX)
        );
        _rightVolume = static_cast<int>(
            std::max(-1.0f, std::min(1.0f, right)) * static_cast<float>(INT16_MAX)
        );
    }
    inline void setVolume(float value) {
        setVolume(value, value);
    }

    template<typename T> inline void feed(const T *data, int size) {
        SDL_AudioStreamPut(_stream, data, size);
    }
    inline void feed(AudioBuffer &buffer) {
        SDL_AudioStreamPut(_stream, buffer.data.data(), buffer.data.size());
    }

    inline void close(void) {
        // Do not actually destroy the stream, instead just flush it to make sure all leftover data is played.
        _busy = false;
        if (_stream)
            SDL_AudioStreamFlush(_stream);
    }
};

class Mixer {
private:
    MixerStream _streams[NUM_CHANNELS];

    int _leftVolume, _rightVolume;
    int _sampleRate;
    uint64_t _sampleOffset;

public:
    inline Mixer(int sampleRate)
    : _leftVolume(INT16_MAX), _rightVolume(INT16_MAX), _sampleRate(sampleRate), _sampleOffset(0) {}

    inline void setMasterVolume(float left, float right) {
        _leftVolume = static_cast<int>(
            std::max(-1.0f, std::min(1.0f, left)) * static_cast<float>(INT16_MAX)
        );
        _rightVolume = static_cast<int>(
            std::max(-1.0f, std::min(1.0f, right)) * static_cast<float>(INT16_MAX)
        );
    }
    inline void setMasterVolume(float value) {
        setMasterVolume(value, value);
    }
    inline float getLatency(int numSamples) {
        return static_cast<float>(numSamples) * static_cast<float>(_sampleRate);
    }

    MixerStream *openStream(SDL_AudioFormat format, int channels, int sampleRate);
    void process(int16_t *output, int numSamples);
};

MixerStream *Mixer::openStream(SDL_AudioFormat format, int channels, int sampleRate) {
    for (int ch = 0; ch < NUM_CHANNELS; ch++) {
        auto &stream = _streams[ch];
        if (stream.isBusy())
            continue;

        stream._open(format, channels, sampleRate, _sampleRate);
        return &stream;
    }

    return nullptr;
}

Mixer *audioMixer;

// The output format is hardcoded to 16 bit stereo.
void Mixer::process(int16_t *output, int numSamples) {
    int16_t inputBuffer[MAX_BUFFER_SIZE][2];
    int32_t outputBuffer[MAX_BUFFER_SIZE][2];

    int inputBufferSize = numSamples * 2 * sizeof(int16_t);
    int outputBufferSize = numSamples * 2 * sizeof(int32_t);

    assert(numSamples <= MAX_BUFFER_SIZE);
    memset(outputBuffer, 0, outputBufferSize);
    _sampleOffset += numSamples;

    for (int ch = 0; ch < NUM_CHANNELS; ch++) {
        auto &stream = _streams[ch];
        if (!stream.isBusy())
            continue;

        int actualNumSamples = stream._read(&inputBuffer[0][0], inputBufferSize);

        // Apply volume and mix into output buffer
        for (int i = 0; i < actualNumSamples; i++) {
            outputBuffer[i][0] += (inputBuffer[i][0] * stream._leftVolume) >> 16;
            outputBuffer[i][1] += (inputBuffer[i][1] * stream._rightVolume) >> 16;
        }

    }

    // Apply master volume, clip and copy to output buffer
    for (int i = 0; i < numSamples; i++) {
        output[i * 2 + 0] = static_cast<int16_t>(
            std::max(INT16_MIN, std::min(INT16_MAX, (int)(outputBuffer[i][0] * _leftVolume) >> 16))
        );
        output[i * 2 + 1] = static_cast<int16_t>(
            std::max(INT16_MIN, std::min(INT16_MAX, (int)(outputBuffer[i][0] * _rightVolume) >> 16))
        );
    }
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

    assert(_readValue<uint32_t>(wavFile) == "RIFF"_m);
    _readValue<uint32_t>(wavFile); //ignore total file size
    assert(_readValue<uint32_t>(wavFile) == "WAVE"_m);

    uint32_t chunkType = _readValue<uint32_t>(wavFile);
    uint32_t chunkLength = _readValue<uint32_t>(wavFile);

    while (!feof(wavFile)) {
        if (chunkType == "fmt "_m) {
            _readValueInPlace(wavFile, fmt);
            audioMixer->openStream(format, fmt.channels, fmt.samplerate);
            assert(fmt.format == 1); // 1 is int PCM, 3 is float PCM, 0x11 is ADPCM

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
    return NULL;

    StreamSource *source;
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