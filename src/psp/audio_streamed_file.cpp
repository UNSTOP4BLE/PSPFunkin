
#include <algorithm>
#include "../main.h"
#include "audio_streamed_file.h"

namespace Audio {

StreamedFile::StreamedFile(Mixer &mixer, const char *path)
: _playing(false), _loopOffset(-1) {
    _reader = openFile(path);
    ASSERTFUNC(_reader, "failed to open audio file");

    _channel = mixer.openChannel(_reader->format, _reader->channels, _reader->sampleRate);
    ASSERTFUNC(_channel, "no mixer channels available to play stream");
}

/*
 * maxFillLength can be used to limit the maximum number of samples that will be
 * decoded for each call to process(). This is useful for example if calling
 * process() once per frame as part of the main loop, to prevent audio decoding
 * from taking too much CPU time and eating into the time budget for the frame.
 *
 * Note that maxFillLength and DEFAULT_MAX_FILL_LENGTH must be high enough to
 * prevent buffer underruns. For example, if process() is called 60 times per
 * second to play a 44100 Hz stream, it must at the very least be 44100 / 60 =
 * 735 samples (but ideally should be more to prevent frame rate drops from
 * interrupting the stream).
 */
void StreamedFile::process(int maxFillLength) {
    if (!_playing)
        return;

    int length = STREAM_BUFFER_SIZE - _channel->getBufferedSamples();
    if ((maxFillLength > 0) && (maxFillLength < length))
        length = maxFillLength;

    for (int offset = 0; offset < length;) {
        offset += _reader->read(_buffer, maxFillLength, offset);

        if (_reader->getPosition() >= _reader->totalNumSamples) {
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

    // Note that feed() is highly expensive as it requires locking the audio
    // thread's mutex, so it should only be called once.
    _channel->feed(_buffer);
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
