#pragma once

#include <cstdint>
#include <chrono>
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_mutex.h>
#include "../main.h"
#include "audio_buffer.h"

namespace Audio {

// 1024-sample buffer at 44100 Hz = ~23ms latency
static constexpr int NUM_MIXER_CHANNELS = 16;
static constexpr int MIXER_BUFFER_SIZE = 1024;

class Mixer;

class MixerChannel {
    friend class Mixer;

private:
    Mixer *_mixer;
    SDL_AudioStream *_stream;
    int _leftVolume, _rightVolume; // fixed-point (1 << VOLUME_SHIFT)
    int _sampleRate;
    bool _busy;
    // Atomics are not supported properly by PSPSDK, so volatile it is.
    volatile int64_t _sampleOffset;

    // These are only accessible to the mixer class.
    inline MixerChannel(void)
    : _stream(nullptr), _busy(false) {}
    inline ~MixerChannel(void) {
        if (_stream)
            SDL_FreeAudioStream(_stream);
    }

    void _open(SDL_AudioFormat format, int channels, int sampleRate, int mixerSampleRate);
    int _read(int16_t *data, int size);
    int _getBufferedSamples(void);

public:
    void feed(const void *data, int size);
    void feed(AudioBuffer &buffer);
    int getBufferedSamples(void);
    int cancelPlayback(void);

    void setVolume(float left, float right);
    int64_t getSampleOffset(void);
    void setSampleOffset(int64_t value);

    bool isBusy(void);
    void close(void);
};

class Mixer {
    friend class MixerChannel;

private:
    SDL_AudioDeviceID _outputStream;
    SDL_mutex *_sampleOffsetMutex;
    MixerChannel _channels[NUM_MIXER_CHANNELS];

    int _leftVolume, _rightVolume;
    int _sampleRate;
    volatile int64_t _sampleOffset;
    volatile std::chrono::high_resolution_clock::time_point _sampleOffsetTimestamp;

    inline void _lockCallback(void) {
        SDL_LockAudioDevice(_outputStream);
    }
    inline void _unlockCallback(void) {
        SDL_UnlockAudioDevice(_outputStream);
    }
    inline void _lockSampleOffset(void) {
        SDL_LockMutex(_sampleOffsetMutex);
    }
    inline void _unlockSampleOffset(void) {
        SDL_UnlockMutex(_sampleOffsetMutex);
    }

    void _process(int16_t *output, int numSamples);

public:
    Mixer(void);
    ~Mixer(void);

    void start(int sampleRate = 44100, int bufferSize = MIXER_BUFFER_SIZE);
    void stop(void);

    MixerChannel *openChannel(SDL_AudioFormat format, int channels, int sampleRate);
    MixerChannel *playBuffer(AudioBuffer &buffer, bool close = true);
    void setMasterVolume(float left, float right);
    int64_t getSampleOffset(void);
};

}
