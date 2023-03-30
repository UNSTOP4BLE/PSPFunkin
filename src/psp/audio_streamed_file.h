#pragma once

#include "../main.h"
#include "audio_buffer.h"
#include "audio_file_readers.h"
#include "audio_mixer.h"

namespace Audio {

static constexpr int STREAM_BUFFER_SIZE = 4096;

class StreamedFile {
public:
    StreamedFile(const char *path);
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

    void process(void); // must be called to make sure the file keeps playing
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