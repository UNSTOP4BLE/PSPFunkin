#pragma once

#include "psp/gfx.h"

class Screen {
public:
    inline Screen(void) {}
    virtual void update(void) {}
    virtual void draw(void) {}
    virtual ~Screen(void) {}
};

void setScreen(Screen *scr);
void setScreenCol(uint32_t color);