#include "../main.h"
#include "../screen.h"
#include "../psp/font.h"
#include "../psp/pad.h"

MainMenuScreen::MainMenuScreen(void) {
    setScreenCol(GREEN);
    background = g2dTexLoad("assets/menu/back.png", G2D_SWIZZLE);
    //backgroundy.setValue(G2D_SCR_H/2 - 331/2);
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
    Rect background_img = {0, 0, 512, 331};
    FRect background_disp = {G2D_SCR_W/2 - 583/2, backgroundy.getValue(), 583, 331};
    DrawFG2DTex(background, &background_img, &background_disp, true, 0, 255);
}

MainMenuScreen::~MainMenuScreen(void) 
{
    g2dTexFree(&background);
}