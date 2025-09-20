//done
#include "audiomanager.hpp"

namespace MANAGER {

void AudioManager::loadFile(AUDIO::Mixer &mixer, const std::string& path) { //todo make stuff use const referances so it doesnt copy crap for no reason
    if (file.path == path) {
        printf("%s already loaded, skipping \n", path.c_str());
        return; // same file already loaded, do nothing
    }
    printf("loading %s \n", path.c_str());
    file.stream = std::make_unique<AUDIO::StreamedFile>(mixer, path.c_str()); //todo make this take string?
    file.path = path;
} 

void AudioManager::update(void) {
    //process all streams
    if (file.stream)
        file.stream->process();
}

void AudioManager::play(bool loop) {
    if (file.stream)
        file.stream->play(loop);
}

float AudioManager::getMS(void) {
    if (file.stream)
        return file.stream->getChannel().getTime()*1000;
    return 0;
}

}