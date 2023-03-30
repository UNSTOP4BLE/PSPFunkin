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
        _stream->close();
        delete _reader;
    }

    inline MixerStream &getStream(void) {
        return *_stream;
    }
    inline int getPosition(void) {
        return _reader->getPosition();
    }
    inline int setPosition(int sampleOffset) {
        return _reader->setPosition(sampleOffset);
    }
    inline bool isPlaying(void) {
        return _playing;
    }

    void process(void); // must be called to make sure the file keeps playing
    void play(int loopOffset = -1);
    void pause(void);
    //float getTime(void);
    //float setTime(float time);

private:
    AudioBuffer _buffer;
    MixerStream *_stream;
    FileReader *_reader;
    bool _playing;
    int _loopOffset;
};

}
