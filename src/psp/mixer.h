#pragma once
#include "audio.h"
#include "../main.h"
#include <cstdint>
#include <algorithm>
#include <SDL2/SDL.h>

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

        ASSERTFUNC(_stream);
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
        // Do not actually destroy the stream, instead just flush it to make
        // sure all leftover data is played.
        _busy = false;
        if (_stream)
            SDL_AudioStreamFlush(_stream);
    }
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
    : _outputStream(0), _leftVolume(INT16_MAX), _rightVolume(INT16_MAX),
        _sampleRate(44100), _sampleOffset(0) {}

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

    inline void lock(void) {
        SDL_LockAudioDevice(_outputStream);
    }
    inline void unlock(void) {
        SDL_UnlockAudioDevice(_outputStream);
    }

    void start(int sampleRate = 44100, int bufferSize = MAX_BUFFER_SIZE);
    void stop(void);

    MixerStream *openStream(SDL_AudioFormat format, int channels, int sampleRate);
    void process(int16_t *output, int numSamples);
};