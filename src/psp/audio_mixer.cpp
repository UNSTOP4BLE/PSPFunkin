
#include <cstdint>
#include <algorithm>
#include <chrono>
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_mutex.h>
#include "../main.h"
#include "audio_mixer.h"

/*
 * Note about Mixer::getSampleOffset():
 *
 *  Buffer processed: ---AAABBB-------AAA-------BBB------- ...
 *  Buffer played:    ------AAAAAAAAAABBBBBBBBBBAAAAAAAAAA ...
 *  _sampleOffset:   -2 -1  0         1         2          ...
 *
 *  _sampleOffset is initialized to -(2 * bufferSize) by start() and then
 *  incremented twice, first when process() is called initially to prefill the
 *  first buffer and then when SDL actually starts playing the output stream and
 *  process() is invoked again to fill the next buffer. Thus, by the time
 *  _sampleOffset is 0 the audio has started playing and _sampleOffset always
 *  reflects the offset of the currently playing buffer. All that's left to do
 *  is some linear interpolation using the delta between the current time and
 *  the time _sampleOffset was last updated.
 *
 *  The same thing is done in MixerChannel::getSampleOffset(), except
 *  _sampleOffset has to be converted from the mixer's sample rate to the
 *  channel's sample rate first.
 */

#define _seconds std::chrono::duration<float, std::ratio<1>>
#define _timePointType std::chrono::high_resolution_clock::time_point

// Horrible but as long as it does the job...
static volatile _timePointType _getCurrentTime(void) {
    return static_cast<volatile _timePointType>(std::chrono::high_resolution_clock::now());
}

namespace Audio {

void MixerChannel::_open(SDL_AudioFormat format, int channels, int sampleRate, int mixerSampleRate) {
    _mixer->_lockCallback();

    _leftVolume = INT16_MAX;
    _rightVolume = INT16_MAX;
    _sampleRate = sampleRate;
    _busy = true;
    _sampleOffset = 0;

    if (_stream)
        SDL_FreeAudioStream(_stream);

    _stream = SDL_NewAudioStream(
        format, channels, sampleRate, // input
        AUDIO_S16SYS, 2, mixerSampleRate // output
    );

    _mixer->_unlockCallback();
    ASSERTFUNC(_stream, "failed to allocate audio stream");
}

int MixerChannel::_read(int16_t *data, int size) {
    if (!_stream)
        return 0;

    return SDL_AudioStreamGet(_stream, data, size) / (2 * sizeof(int16_t));
}

int MixerChannel::_getBufferedSamples(void) {
    if (!_stream)
        return 0;

    return SDL_AudioStreamAvailable(_stream) / (2 * sizeof(int16_t));
}

void MixerChannel::feed(const void *data, int size) {
    _mixer->_lockCallback();
    SDL_AudioStreamPut(_stream, data, size);
    _mixer->_unlockCallback();
}

void MixerChannel::feed(AudioBuffer &buffer) {
    feed(buffer.data.data(), buffer.data.size());
}

int MixerChannel::getBufferedSamples(void) {
    _mixer->_lockCallback();
    int samples = _getBufferedSamples();
    _mixer->_unlockCallback();

    return samples;
}

int MixerChannel::cancelPlayback(void) {
    _mixer->_lockCallback();
    int remaining = _getBufferedSamples();
    SDL_AudioStreamClear(_stream);
    _mixer->_unlockCallback();

    return remaining;
}

void MixerChannel::setVolume(float left, float right) {
    _mixer->_lockCallback();

    _leftVolume = static_cast<int>(
        std::max(-1.0f, std::min(1.0f, left)) * static_cast<float>(INT16_MAX)
    );
    _rightVolume = static_cast<int>(
        std::max(-1.0f, std::min(1.0f, right)) * static_cast<float>(INT16_MAX)
    );

    _mixer->_unlockCallback();
}

int64_t MixerChannel::getSampleOffset(void) {
    _mixer->_lockSampleOffset();

    int64_t offset = _sampleOffset * static_cast<int64_t>(_sampleRate) / static_cast<int64_t>(_mixer->_sampleRate);
    auto delta = std::chrono::duration_cast<_seconds>(
        _getCurrentTime() - _mixer->_sampleOffsetTimestamp
    );

    _mixer->_unlockSampleOffset();
    return offset + static_cast<int64_t>(delta.count() * static_cast<float>(_sampleRate));
}

void MixerChannel::setSampleOffset(int64_t value) {
    int offset = value * static_cast<int64_t>(_mixer->_sampleRate) / static_cast<int64_t>(_sampleRate);

    _mixer->_lockSampleOffset();
    _sampleOffset = offset; // TODO: add offset to compensate for buffering
    _mixer->_unlockSampleOffset();
}

bool MixerChannel::isBusy(void) {
    if (!_stream)
        return false;
    if (_busy)
        return true;
    if (getBufferedSamples())
        return true;

    return false;
}

void MixerChannel::close(void) {
    // Do not actually destroy the stream, instead just flush it to make
    // sure all leftover data is played.
    _busy = false;

    if (_stream) {
        _mixer->_lockCallback();
        SDL_AudioStreamFlush(_stream);
        _mixer->_unlockCallback();
    }
}

Mixer::Mixer(void)
: _outputStream(0), _leftVolume(INT16_MAX), _rightVolume(INT16_MAX) {
    for (int ch = 0; ch < NUM_MIXER_CHANNELS; ch++)
        _channels[ch]._mixer = this;

    _sampleOffsetMutex = SDL_CreateMutex();
    ASSERTFUNC(_sampleOffsetMutex, "failed to create mutex");
}

Mixer::~Mixer(void) {
    stop();
    SDL_DestroyMutex(_sampleOffsetMutex);
}

// The output format is hardcoded to 16 bit stereo.
void Mixer::_process(int16_t *output, int numSamples) {
    int16_t inputBuffer[MIXER_BUFFER_SIZE][2];
    int32_t outputBuffer[MIXER_BUFFER_SIZE][2];

    // Update sample offsets (as fast as possible to avoid introducing delays)
    _lockSampleOffset();
    _sampleOffset += numSamples;
    _sampleOffsetTimestamp = _getCurrentTime();

    for (int ch = 0; ch < NUM_MIXER_CHANNELS; ch++) {
        auto &channel = _channels[ch];
        channel._sampleOffset += channel._getBufferedSamples();
    }

    _unlockSampleOffset();

    // Initialize output buffer and mix all channels
    int inputBufferSize = numSamples * 2 * sizeof(int16_t);
    int outputBufferSize = numSamples * 2 * sizeof(int32_t);
    memset(outputBuffer, 0, outputBufferSize);

    for (int ch = 0; ch < NUM_MIXER_CHANNELS; ch++) {
        auto &channel = _channels[ch];
        int bufferedSamples = channel._read(&inputBuffer[0][0], inputBufferSize);

        // Apply volume and mix into output buffer
        for (int i = 0; i < bufferedSamples; i++) {
            outputBuffer[i][0] += (inputBuffer[i][0] * channel._leftVolume) >> 16;
            outputBuffer[i][1] += (inputBuffer[i][1] * channel._rightVolume) >> 16;
        }
    }

    // Apply master volume, clip and copy to SDL buffer
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
    _sampleOffset = -(2 * bufferSize); // See note
    SDL_PauseAudioDevice(_outputStream, false);
}

void Mixer::stop(void) {
    if (_outputStream) {
        SDL_CloseAudioDevice(_outputStream);
        _outputStream = 0;
    }
}

MixerChannel *Mixer::openChannel(SDL_AudioFormat format, int channels, int sampleRate) {
    for (int ch = 0; ch < NUM_MIXER_CHANNELS; ch++) {
        auto &channel = _channels[ch];
        if (channel.isBusy())
            continue;

        channel._open(format, channels, sampleRate, _sampleRate);
        return &channel;
    }

    return nullptr;
}

MixerChannel *Mixer::playBuffer(AudioBuffer &buffer, bool close) {
    auto channel = openChannel(buffer.format, buffer.channels, buffer.sampleRate);
    if (channel) {
        channel->feed(buffer);
        if (close)
            channel->close();
    }

    return channel;
}

void Mixer::setMasterVolume(float left, float right) {
    _lockCallback();

    _leftVolume = static_cast<int>(
        std::max(-1.0f, std::min(1.0f, left)) * static_cast<float>(INT16_MAX)
    );
    _rightVolume = static_cast<int>(
        std::max(-1.0f, std::min(1.0f, right)) * static_cast<float>(INT16_MAX)
    );

    _unlockCallback();
}

int64_t Mixer::getSampleOffset(void) {
    _lockSampleOffset();

    int64_t offset = _sampleOffset;
    auto delta = std::chrono::duration_cast<_seconds>(
        _getCurrentTime() - _sampleOffsetTimestamp
    );

    _unlockSampleOffset();
    return offset + static_cast<int64_t>(delta.count() * static_cast<float>(_sampleRate));
}

}
