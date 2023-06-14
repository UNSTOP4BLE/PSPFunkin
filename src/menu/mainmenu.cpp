#include "../main.h"
#include "../screen.h"
#include "../psp/font.h"
#include "../psp/pad.h"

MainMenuScreen::MainMenuScreen(void) {
    setScreenCol(0xFF00FF00);
    background = GFX::loadTex("assets/menu/back.png");
    //backgroundy.setValue(GFX::SCREEN_HEIGHT/2 - 331/2);
}

void MainMenuScreen::update(void) 
{
    if (Pad_Pressed(PSP_CTRL_CROSS | PSP_CTRL_START))
        setScreen(new PlayStateScreen());
    else if (Pad_Pressed(PSP_CTRL_UP))
    {
        selection -= 1;
//        backgroundy.setValue(5 * selection, 10);
    }
    else if (Pad_Pressed(PSP_CTRL_DOWN))    
    {
        selection += 1;
  //      backgroundy.setValue(5 * selection, 10);
    }
}

void MainMenuScreen::draw(void) 
{
    GFX::RECT<int> background_img = {0, 0, 512, 331};
    GFX::RECT<float> background_disp = {GFX::SCREEN_WIDTH/2 - 583/2, backgroundy.getValue(), 583, 331};
    GFX::drawTex<float>(background, &background_img, &background_disp, true, 0, 255);
}

MainMenuScreen::~MainMenuScreen(void) 
{
    GFX::freeTex(background);
}