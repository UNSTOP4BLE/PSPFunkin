#include "title.hpp"
#include "../engine/pc/renderersdl.hpp"

TitleSCN::TitleSCN(void) {
    freaky = new Audio::StreamedFile(g_app.audiomixer, getFilePath("assets/music/freaky.ogg").c_str());
    freaky->play(true);
    test = FS::loadTexFile(getFilePath("assets/menu/options.png"));
}

void TitleSCN::update(void) {
    freaky->process();
}

void TitleSCN::draw(void) {
 //   SDL_RenderCopy(GFX::g_sdlrenderer, test.tex, nullptr, nullptr); //works
    g_app.renderer->drawTexRect(test, {0, 0, 100, 100}, {0, 0, 100, 100}, 0, 0xFFFFFFFF); //doesnt draw anything
}

TitleSCN::~TitleSCN(void) {
    delete freaky;
}  