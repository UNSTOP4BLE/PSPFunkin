
#include "../main.h"
#include "audio_streamed_file.h"

namespace Audio {

void StreamedFile::process(void) {
    if (!playing) return;

    while (stream.getBufferedSamples() < STREAM_BUFFER_SIZE) {
        int length = reader->read(buffer, STREAM_BUFFER_SIZE);
        stream.feed(buffer);

        if (length < STREAM_BUFFER_SIZE) {
            if (loopOffset >= 0) {
                // Do seamless looping (keep filling the stream buffer)
                setPosition(loopOffset);
            } else {
                // Stop playback
                playing = false;
                break;
            }
        }
    }
}

void StreamedFile::play(int loopPoint) {
    playing = true;
    loopOffset = loopPoint;

    //process();
}

void StreamedFile::pause(void) {
    playing = false;

    // Make sure any sample that wasn't played is going to be played again
    int offset = stream.cancelPlayback();
    setPosition(getPosition() - offset);
}

}
