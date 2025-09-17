#pragma once

#include "../app.hpp"
#include "../engine/audio_streamed_file.hpp"

class TitleSCN : public SCENE::Scene {
public:
    TitleSCN(void);
    void update(void);
    void draw(void);
    ~TitleSCN(void); 
private:
    Audio::StreamedFile *freaky; 
    GFX::Texture test;
};