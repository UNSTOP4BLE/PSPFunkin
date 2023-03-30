#pragma once

#include <cstdint>
#include <algorithm>
#include <chrono>
#include <SDL2/SDL_audio.h>
#include "../main.h"
#include "audio_buffer.h"

namespace Audio {

// 1024-sample buffer at 44100 Hz = ~23ms latency
static constexpr int NUM_MIXER_CHANNELS = 32;
static constexpr int MIXER_BUFFER_SIZE = 1024;

class Mixer;

class MixerStream {
    friend class Mixer;

private:
    Mixer *_mixer;
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
        //_mixer->enterCriticalSection();
        _leftVolume = static_cast<int>(
            std::max(-1.0f, std::min(1.0f, left)) * static_cast<float>(INT16_MAX)
        );
        _rightVolume = static_cast<int>(
            std::max(-1.0f, std::min(1.0f, right)) * static_cast<float>(INT16_MAX)
        );
        //_mixer->exitCriticalSection();
    }
    inline void setVolume(float value) {
        setVolume(value, value);
    }

    void feed(const void *data, int size);
    void feed(AudioBuffer &buffer);
    int getBufferedSamples(void);
    int cancelPlayback(void);
    bool isBusy(void);
    void close(void);
};

class Mixer {
private:
    SDL_AudioDeviceID _outputStream;
    MixerStream _streams[NUM_MIXER_CHANNELS];

    int _leftVolume, _rightVolume;
    int _sampleRate;
    int64_t _sampleOffset;
    std::chrono::high_resolution_clock::time_point _sampleOffsetTimestamp;

    void _process(int16_t *output, int numSamples);

public:
    bool playingStreams[NUM_MIXER_CHANNELS];

    Mixer(void);
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

    void start(int sampleRate = 44100, int bufferSize = MIXER_BUFFER_SIZE);
    void stop(void);

    MixerStream *openStream(SDL_AudioFormat format, int channels, int sampleRate);
    MixerStream *playBuffer(AudioBuffer &buffer, bool close = true);
    bool isStreaming(void); 
    int64_t getSampleOffset(void);
};

}
