#pragma once

#include "../main.h"
#include "audio_buffer.h"
#include "audio_file_readers.h"
#include "audio_mixer.h"

namespace Audio {

static constexpr int STREAM_BUFFER_SIZE = 4096;

class StreamedFile {
public:
    inline StreamedFile(MixerStream &stream, const char *path)
    : stream(stream), playing(false), loopOffset(-1) {
        reader = openFile(path);
        ASSERTFUNC(reader);
    }
    inline ~StreamedFile(void) {
        delete reader;
    }

    inline int getPosition(void) {
        return reader->getPosition();
    }
    inline int setPosition(int sampleOffset) {
        return reader->setPosition(sampleOffset);
    }
    inline bool isPlaying(void) {
        return playing;
    }

    void process(void); // must be called to make sure the file keeps playing
    void play(int loopPoint = -1);
    void pause(void);
    //float getTime(void);
    //float setTime(float time);

private:
    AudioBuffer buffer;
    MixerStream &stream;
    FileReader *reader;
    bool playing;
    int loopOffset;
};

}
