#include "../main.h"
#include "audioreaders.h"

class StreamedFile {
public:
    StreamedFile(MixerStream &stream, const char *path);
    ~StreamedFile(void);
    void play(void);
    void stop(void);
    int setPosition(int sampleOffset);
    int getPosition(void);
    float setTime(float time);
    float getTime(void);
private:
    MixerStream &stream;
    Audio::FileReader *file;

};

StreamedFile::StreamedFile(MixerStream &stream, const char *path) {
    file = fopen(path, "rb")
}