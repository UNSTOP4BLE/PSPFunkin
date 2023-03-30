
#include "../main.h"
#include "audio_streamed_file.h"

namespace Audio {

StreamedFile::StreamedFile(const char *path)
: _playing(false), _loopOffset(-1) {
    _reader = openFile(path);
    ASSERTFUNC(_reader, "failed to open audio file");

    _stream = app->audioMixer->openStream(_reader->format, _reader->channels, _reader->sampleRate);
    ASSERTFUNC(_stream, "no mixer channels available to play stream");
}

void StreamedFile::process(void) {
    if (!_playing) return;

    while (_stream->getBufferedSamples() < STREAM_BUFFER_SIZE) {
        int length = _reader->read(_buffer, STREAM_BUFFER_SIZE);
        _stream->feed(_buffer);

        if (length < STREAM_BUFFER_SIZE) {
            if (_loopOffset >= 0) {
                // Do seamless looping (keep filling the stream buffer)
                setPosition(_loopOffset);
            } else {
                // Stop playback
                _playing = false;
                app->audioMixer->playingStreams[chan] = false;
                break;
            }
        }
    }
}

void StreamedFile::play(int loopOffset) {
    _playing = true;
    app->audioMixer->playingStreams[chan] = true;

    _loopOffset = loopOffset;
}

void StreamedFile::pause(void) {
    _playing = false;

    // Make sure any sample that wasn't played is going to be played again
    int offset = _stream->cancelPlayback();
    setPosition(getPosition() - offset);
}

}
