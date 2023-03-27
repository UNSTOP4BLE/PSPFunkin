#include "mixer.h"
#include "../main.h"

void Mixer::start(int sampleRate, int bufferSize) {
    ASSERTFUNC(bufferSize <= MAX_BUFFER_SIZE);

    SDL_AudioSpec actualSpec;
    SDL_AudioSpec spec{
        .freq     = sampleRate,
        .format   = AUDIO_S16SYS,
        .channels = 2,
        .samples  = (uint16_t)bufferSize,
        .callback = [](Mixer *mixer, int16_t *output, int size) {
            mixer->process(output, size / (2 * sizeof(int16_t)));
        },
        .userdata = this
    };

    _outputStream = SDL_OpenAudioDevice(
        nullptr, false, &spec, &actualSpec, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE
    );
    ASSERTFUNC(_outputStream);

    _sampleRate = actualSpec.freq; // May have been changed by SDL
    SDL_PauseAudioDevice(_outputStream, false);
}

void Mixer::stop(void) {
    if (_outputStream)
        SDL_CloseAudioDevice(_outputStream);
}

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

// The output format is hardcoded to 16 bit stereo.
void Mixer::process(int16_t *output, int numSamples) {
    int16_t inputBuffer[MAX_BUFFER_SIZE][2];
    int32_t outputBuffer[MAX_BUFFER_SIZE][2];

    int inputBufferSize = numSamples * 2 * sizeof(int16_t);
    int outputBufferSize = numSamples * 2 * sizeof(int32_t);

    ASSERTFUNC(numSamples <= MAX_BUFFER_SIZE);
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