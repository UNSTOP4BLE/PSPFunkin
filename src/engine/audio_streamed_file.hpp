//done
#pragma once

#include "audio_buffer.hpp"
#include "audio_file_readers.hpp"
#include "audio_mixer.hpp"

namespace AUDIO {

static constexpr int STREAM_BUFFER_SIZE = 4096;
static constexpr int DEFAULT_MAX_FILL_LENGTH = 2048; // See note in the source file

class StreamedFile {
public:
    StreamedFile(Mixer &mixer, const char *path);
    inline ~StreamedFile(void) {
        _channel->close();
        delete _reader;
    }

    inline MixerChannel &getChannel(void) {
        return *_channel;
    }
    inline int getPosition(void) {
        return _reader->getPosition();
    }
    inline int setPosition(int sampleOffset) {
        int value = _reader->setPosition(sampleOffset);
        _channel->setSampleOffset(value);
        return value;
    }
    inline bool isPlaying(void) {
        return _playing;
    }

    inline void setVolume(float left, float right) {
        _channel->setVolume(left, right);
    }

    void process(int maxFillLength = DEFAULT_MAX_FILL_LENGTH);
    void play(int loopOffset = -1);
    void pause(void);
    int64_t getSampleOffset(void);

private:
    AudioBuffer _buffer;
    MixerChannel *_channel;
    FileReader *_reader;
    bool _playing;
    int _loopOffset;
};

}
