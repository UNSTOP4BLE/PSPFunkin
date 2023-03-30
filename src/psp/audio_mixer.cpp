#include <cstdint>
#include <algorithm>
#include <chrono>
#include <SDL2/SDL_audio.h>
#include <stdint.h>
#include "../main.h"
#include "audio_mixer.h"

namespace Audio {

void MixerStream::_open(SDL_AudioFormat format, int channels, int sampleRate, int mixerSampleRate) {
    _mixer->enterCriticalSection();

    _leftVolume = INT16_MAX;
    _rightVolume = INT16_MAX;
    _busy = true;

    if (_stream)
        SDL_FreeAudioStream(_stream);

    _stream = SDL_NewAudioStream(
        format, channels, sampleRate, // input
        AUDIO_S16SYS, 2, mixerSampleRate // output
    );

    _mixer->exitCriticalSection();
    ASSERTFUNC(_stream, "failed to allocate audio stream");
}

int MixerStream::_read(int16_t *data, int size) {
    if (!_stream)
        return 0;

    return SDL_AudioStreamGet(_stream, data, size);
}

void MixerStream::feed(const void *data, int size) {
    _mixer->enterCriticalSection();
    SDL_AudioStreamPut(_stream, data, size);
    _mixer->exitCriticalSection();
}

void MixerStream::feed(AudioBuffer &buffer) {
    feed(buffer.data.data(), buffer.data.size());
}

int MixerStream::getBufferedSamples(void) {
    _mixer->enterCriticalSection();
    int samples = SDL_AudioStreamAvailable(_stream) / (2 * sizeof(int16_t));
    _mixer->exitCriticalSection();

    return samples;
}

int MixerStream::cancelPlayback(void) {
    _mixer->enterCriticalSection();
    int remaining = getBufferedSamples();
    SDL_AudioStreamClear(_stream);
    _mixer->exitCriticalSection();

    return remaining;
}

bool MixerStream::isBusy(void) {
    if (!_stream)
        return false;
    if (_busy)
        return true;
    if (getBufferedSamples())
        return true;

    return false;
}

void MixerStream::close(void) {
    // Do not actually destroy the stream, instead just flush it to make
    // sure all leftover data is played.
    _busy = false;

    if (_stream) {
        _mixer->enterCriticalSection();
        SDL_AudioStreamFlush(_stream);
        _mixer->exitCriticalSection();
    }
}

Mixer::Mixer(void)
: _outputStream(0), _leftVolume(INT16_MAX), _rightVolume(INT16_MAX) {
    for (int ch = 0; ch < NUM_MIXER_CHANNELS; ch++)
        _streams[ch]._mixer = this;
}

// The output format is hardcoded to 16 bit stereo.
void Mixer::_process(int16_t *output, int numSamples) {
    int16_t inputBuffer[MIXER_BUFFER_SIZE][2];
    int32_t outputBuffer[MIXER_BUFFER_SIZE][2];

    _sampleOffset += numSamples;
    _sampleOffsetTimestamp = std::chrono::high_resolution_clock::now();

    int inputBufferSize = numSamples * 2 * sizeof(int16_t);
    int outputBufferSize = numSamples * 2 * sizeof(int32_t);
    memset(outputBuffer, 0, outputBufferSize);

    for (int ch = 0; ch < NUM_MIXER_CHANNELS; ch++) {
        auto &stream = _streams[ch];
        int actualNumSamples = stream._read(&inputBuffer[0][0], inputBufferSize) / (2 * sizeof(int16_t));

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

void Mixer::start(int sampleRate, int bufferSize) {
    ASSERTFUNC(bufferSize <= MIXER_BUFFER_SIZE, "unsupported buffer size");

    SDL_AudioSpec actualSpec;
    SDL_AudioSpec spec{
        .freq     = sampleRate,
        .format   = AUDIO_S16SYS,
        .channels = 2,
        .samples  = static_cast<uint16_t>(bufferSize),
        .callback = [](void *userdata, uint8_t *buffer, int size) {
            auto mixer = reinterpret_cast<Mixer *>(userdata);
            auto output = reinterpret_cast<int16_t *>(buffer);

            mixer->_process(output, size / (2 * sizeof(int16_t)));
        },
        .userdata = this
    };

    _outputStream = SDL_OpenAudioDevice(
        nullptr, false, &spec, &actualSpec,
        SDL_AUDIO_ALLOW_FREQUENCY_CHANGE | SDL_AUDIO_ALLOW_SAMPLES_CHANGE
    );
    ASSERTFUNC(_outputStream, "failed to initialize audio device");

    _sampleRate = actualSpec.freq; // May have been changed by SDL
    _sampleOffset = -(bufferSize * 2); // See note above getSampleOffset()
    SDL_PauseAudioDevice(_outputStream, false);
}

void Mixer::stop(void) {
    if (_outputStream) {
        SDL_CloseAudioDevice(_outputStream);
        _outputStream = 0;
    }
}

MixerStream *Mixer::openStream(SDL_AudioFormat format, int channels, int sampleRate) {
    for (int ch = 0; ch < NUM_MIXER_CHANNELS; ch++) {
        auto &stream = _streams[ch];
        if (stream.isBusy())
            continue;

        stream._open(format, channels, sampleRate, _sampleRate);
        return &stream;
    }

    return nullptr;
}

MixerStream *Mixer::playBuffer(AudioBuffer &buffer, bool close) {
    auto stream = openStream(buffer.format, buffer.channels, buffer.sampleRate);
    if (stream) {
        stream->feed(buffer);
        if (close) stream->close();
    }

    return stream;
}

bool Mixer::isStreaming(void) {
    for (int i = 0; i < NUM_MIXER_CHANNELS; i++)
        if (playingStreams[i] == true) return true;
    return false;
}


/*
 * Buffer processed: ---AAABBB-------AAA-------BBB------- ...
 * Buffer played:    ------AAAAAAAAAABBBBBBBBBBAAAAAAAAAA ...
 * _sampleOffset:   -2 -1  0         1         2          ...
 *
 * _sampleOffset is initialized to -(bufferSize * 2) by start() and then
 * incremented twice, first when process() is called initially to prefill the
 * first buffer and then when SDL actually starts playing the output stream and
 * process() is invoked again to fill the next buffer. Thus, by the time
 * _sampleOffset is 0 the audio has started playing and _sampleOffset always
 * reflects the offset of the currently playing buffer. All that's left to do is
 * do some linear interpolation using the delta between the current time and the
 * time _sampleOffset was last updated.
 */

#define _seconds std::chrono::duration<float, std::ratio<1>>

int64_t Mixer::getSampleOffset(void) {
    enterCriticalSection();
    int64_t offset = _sampleOffset;
    auto delta = std::chrono::duration_cast<_seconds>(
        std::chrono::high_resolution_clock::now() - _sampleOffsetTimestamp
    );
    exitCriticalSection();

    return offset + static_cast<int64_t>(delta.count() * static_cast<float>(_sampleRate));
}

}
