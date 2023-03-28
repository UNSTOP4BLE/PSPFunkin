#pragma once
#include "../main.h"
#include "audiobuffer.h"

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

    void _open(SDL_AudioFormat format, int channels, int sampleRate, int mixerSampleRate);
    int _read(int16_t *data, int size);

public:
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

    void feed(const void *data, int size);
    void feed(AudioBuffer *buffer);
    bool isBusy(void);
    void close(void);
};

class Mixer {
private:
    SDL_AudioDeviceID _outputStream;
    MixerStream _streams[NUM_CHANNELS];

    int _leftVolume, _rightVolume;
    int _sampleRate;
    uint64_t _sampleOffset;

public:
    inline Mixer(void)
    : _outputStream(0), _leftVolume(INT16_MAX), _rightVolume(INT16_MAX) {}
    inline ~Mixer(void) {
        stop();
    }

    inline void enterCriticalSection(void) {
        SDL_LockAudioDevice(_outputStream);
    }
    inline void exitCriticalSection(void) {
        SDL_UnlockAudioDevice(_outputStream);
    }

    inline void setMasterVolume(float left, float right) {
        enterCriticalSection();
        _leftVolume = static_cast<int>(
            std::max(-1.0f, std::min(1.0f, left)) * static_cast<float>(INT16_MAX)
        );
        _rightVolume = static_cast<int>(
            std::max(-1.0f, std::min(1.0f, right)) * static_cast<float>(INT16_MAX)
        );
        exitCriticalSection();
    }
    inline void setMasterVolume(float value) {
        setMasterVolume(value, value);
    }
    inline float getLatency(int numSamples) {
        return static_cast<float>(numSamples) * static_cast<float>(_sampleRate);
    }

    void start(int sampleRate = 44100, int bufferSize = MAX_BUFFER_SIZE);
    void stop(void);

    MixerStream *openStream(SDL_AudioFormat format, int channels, int sampleRate);
    void process(int16_t *output, int numSamples);
};
