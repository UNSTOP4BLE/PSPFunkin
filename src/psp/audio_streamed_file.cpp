
#include "../main.h"
#include "audio_streamed_file.h"

namespace Audio {

StreamedFile::StreamedFile(const char *path)
: _playing(false), _loopOffset(-1) {
    _reader = openFile(path);
    ASSERTFUNC(_reader, "failed to open audio file");

    _channel = app->audioMixer->openChannel(_reader->format, _reader->channels, _reader->sampleRate);
    ASSERTFUNC(_channel, "no mixer channels available to play stream");
}

void StreamedFile::process(void) {
    if (!_playing) return;

    while (_channel->getBufferedSamples() < STREAM_BUFFER_SIZE) {
        int length = _reader->read(_buffer, STREAM_BUFFER_SIZE);
        _channel->feed(_buffer);

        if (length < STREAM_BUFFER_SIZE) {
            if (_loopOffset >= 0) {
                // Do seamless looping (keep filling the stream buffer)
                setPosition(_loopOffset);
            } else {
                // Stop playback
                _playing = false;
                break;
            }
        }
    }
}

void StreamedFile::play(int loopOffset) {
    _playing = true;
    _loopOffset = loopOffset;

    _channel->setSampleOffset(getPosition());
}

void StreamedFile::pause(void) {
    _playing = false;

    // Make sure any sample that wasn't played is going to be played again
    int offset = _channel->cancelPlayback();
    setPosition(getPosition() - offset);
}

}