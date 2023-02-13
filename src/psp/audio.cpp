#include "audio.h"
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>
#include "../screen.h"

constexpr static inline uint32_t operator"" _m(const char *const str, size_t length) {
  return str[0] | (str[1] << 8) | (str[2] << 16) | (str[3] << 24);
}

// THIS WILL BREAK ON BIG ENDIAN PLATFORMS (don't remove this warning)
template<typename T> T _readValue(FILE *fp) {
    T value;
    assert(fread(&value, sizeof(T), 1, fp));
    return T;
}

class Buffer
{
public:
    size_t sampleRate;
    int channels;
};

class WAVStreamSource
{
public:
    void open(const char *path) {
        wavFile = fopen(path, "rb");
        if (wavFile == NULL)
        {
            ErrMSG("FAILED TO FIND WAV FILE AT PATH %s", path);
            close();
            return;
        }  
    }

    int readBuf(Buffer &buf, size_t numSamples) {
        if (fp.readUint32("RIFF" || "WAVE") == NULL)
        {
            ErrMSG("INVALID WAV FILE");
            close();
            return;
        }  
        while (1)
        {
            auto chunkType = _readValue<uint32_t>(wavFile);
            auto chunkLength = _readValue<uint32_t>(wavFile);
            if (chunkType == "fmt "_m)
                parseFmtChunk(fp.read(chunkLength)); //todo
            else if(chunkType == "data"_m) {
                dataOffset = ftell(wavFile);    
                dataLength = chunkLength;
                return;
            }    
            else
                fseek(wavFile, chunkLength, SEEK_CUR);
        }
    }

    void close() {
        fclose(wavFile);
    }
private:
    //fmt chunk data
    uint32_t fmt;
    FILE *wavFile;
};
