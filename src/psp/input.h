#pragma once
#include <cstdio> 
#include <cstdint>

namespace Input {

static constexpr int NUM_BUTTONS = 12;

enum Button {
    // In-game controls
    GAME_UP,
    GAME_DOWN,
    GAME_LEFT,
    GAME_RIGHT,
    GAME_PAUSE,

    // Menu controls
    MENU_UP,
    MENU_DOWN,
    MENU_LEFT,
    MENU_RIGHT,
    MENU_ENTER,
    MENU_ESCAPE,
    MENU_OPTION
};
struct Event {
public:
    uint32_t timestamp;
    uint32_t buttonsPressed, buttonsReleased, buttonsHeld;

    inline void reset(void) {
        timestamp = 0;
        buttonsPressed = 0;
        buttonsReleased = 0;
        buttonsHeld = 0;
    }
    inline bool isPressed(Button button) {
        return (buttonsPressed >> button) & 1;
    }
    inline bool isReleased(Button button) {
        return (buttonsReleased >> button) & 1;
    }
    inline bool isHeld(Button button) {
    printf("isHeld %d\n",buttonsHeld);
        return (buttonsHeld >> button) & 1;
    }
};

class Device {
public:
    virtual bool getEvent(Event &output) { (void)output; return false; }
};

class KeyboardDevice : public Device {
private:
    uint32_t _buttonsHeld;

public:
    inline KeyboardDevice(void)
    : _buttonsHeld(0) {}

    bool getEvent(Event &output);
};

class ControllerDevice : public Device {
private:
    uint32_t _buttonsHeld;

public:
    inline ControllerDevice(void)
    : _buttonsHeld(0) {}

    bool getEvent(Event &output);
};


bool windowClosed(void);
}
