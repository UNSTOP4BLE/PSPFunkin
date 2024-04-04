
#include <cstdint>
#include "input.h"

#ifdef PSP
#include <pspctrl.h>
#else
#include <SDL2/SDL.h>
#endif

namespace Input {

#ifdef PSP
static const uint32_t pspMapping[NUM_BUTTONS]{
    PSP_CTRL_UP    | PSP_CTRL_TRIANGLE | PSP_CTRL_RTRIGGER, // GAME_UP
    PSP_CTRL_DOWN  | PSP_CTRL_CROSS | PSP_CTRL_LTRIGGER,    // GAME_DOWN
    PSP_CTRL_LEFT  | PSP_CTRL_SQUARE,   // GAME_LEFT
    PSP_CTRL_RIGHT | PSP_CTRL_CIRCLE,   // GAME_RIGHT
    PSP_CTRL_START,                     // GAME_PAUSE

    PSP_CTRL_UP,     // MENU_UP
    PSP_CTRL_DOWN,   // MENU_DOWN
    PSP_CTRL_LEFT,   // MENU_LEFT
    PSP_CTRL_RIGHT,  // MENU_RIGHT
    PSP_CTRL_CROSS,  // MENU_ENTER
    PSP_CTRL_CIRCLE, // MENU_ESCAPE
    PSP_CTRL_SQUARE  // MENU_OPTION
};
#else
static const int controllerMapping[NUM_BUTTONS][2]{
    { SDL_CONTROLLER_BUTTON_DPAD_UP,    SDL_CONTROLLER_BUTTON_Y }, // GAME_UP
    { SDL_CONTROLLER_BUTTON_DPAD_DOWN,  SDL_CONTROLLER_BUTTON_A }, // GAME_DOWN
    { SDL_CONTROLLER_BUTTON_DPAD_LEFT,  SDL_CONTROLLER_BUTTON_X }, // GAME_LEFT
    { SDL_CONTROLLER_BUTTON_DPAD_RIGHT, SDL_CONTROLLER_BUTTON_B }, // GAME_RIGHT
    { SDL_CONTROLLER_BUTTON_START,      -1 },                      // GAME_PAUSE

    { SDL_CONTROLLER_BUTTON_DPAD_UP,    -1 },                          // MENU_UP
    { SDL_CONTROLLER_BUTTON_DPAD_DOWN,  -1 },                          // MENU_DOWN
    { SDL_CONTROLLER_BUTTON_DPAD_LEFT,  -1 },                          // MENU_LEFT
    { SDL_CONTROLLER_BUTTON_DPAD_RIGHT, -1 },                          // MENU_RIGHT
    { SDL_CONTROLLER_BUTTON_A,          SDL_CONTROLLER_BUTTON_START }, // MENU_ENTER
    { SDL_CONTROLLER_BUTTON_B,          -1 },                          // MENU_ESCAPE
    { SDL_CONTROLLER_BUTTON_X           -1 }                           // MENU_OPTION
};

static const SDL_Scancode keyboardMapping[NUM_BUTTONS]{
    SDL_SCANCODE_UP,     // GAME_UP
    SDL_SCANCODE_DOWN,   // GAME_DOWN
    SDL_SCANCODE_LEFT,   // GAME_LEFT
    SDL_SCANCODE_RIGHT,  // GAME_RIGHT
    SDL_SCANCODE_ESCAPE, // GAME_PAUSE

    SDL_SCANCODE_UP,     // MENU_UP
    SDL_SCANCODE_DOWN,   // MENU_DOWN
    SDL_SCANCODE_LEFT,   // MENU_LEFT
    SDL_SCANCODE_RIGHT,  // MENU_RIGHT
    SDL_SCANCODE_RETURN, // MENU_ENTER
    SDL_SCANCODE_ESCAPE, // MENU_ESCAPE
    SDL_SCANCODE_TAB     // MENU_OPTION
};
#endif

bool KeyboardDevice::getEvent(Event &output) {
    output.reset();
    output.buttonsHeld = _buttonsHeld;

#ifdef PSP
    // PSP has no keyboard
    return false;
#else
    SDL_Event event;
    bool valid = false;

    while (SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_KEYDOWN, SDL_KEYUP) > 0) {
        for (int i = 0; i < NUM_BUTTONS; i++) {
            uint32_t bits = 1 << i;

            if (event.key.keysym.scancode != keyboardMapping[i])
                continue;
            if (event.key.repeat)
                continue;

            if (event.type == SDL_KEYDOWN) {
                output.buttonsPressed |= bits;
                _buttonsHeld |= bits;
            } else {
                output.buttonsReleased |= bits;
                _buttonsHeld &= ~bits;
            }

            output.timestamp = event.key.timestamp;
            output.buttonsHeld = _buttonsHeld;
            valid = true;
        }
    }

    return valid;
#endif
}

bool ControllerDevice::getEvent(Event &output) {
#ifdef PSP
    auto lastHeld = _buttonsHeld;
    _buttonsHeld = 0;

    SceCtrlData state;
    sceCtrlReadBufferPositive(&state, 1);

    for (int i = 0; i < NUM_BUTTONS; i++) {
        if (state.Buttons & pspMapping[i])
            _buttonsHeld |= 1 << i;
    }

    auto changed = _buttonsHeld ^ lastHeld;

    output.timestamp = state.TimeStamp;
    output.buttonsPressed = changed & _buttonsHeld;
    output.buttonsReleased = changed & lastHeld;
    output.buttonsHeld = _buttonsHeld;
    return true;
#else
    output.reset();
    output.buttonsHeld = _buttonsHeld;

    SDL_Event event;
    bool valid = false;

    while (SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_CONTROLLERAXISMOTION, SDL_CONTROLLERBUTTONUP) > 0) {
        for (int i = 0; i < NUM_BUTTONS; i++) {
            uint32_t bits = 1 << i;

            if (
                (event.cbutton.button != controllerMapping[i][0]) &&
                (event.cbutton.button != controllerMapping[i][1])
            )
                continue;

            if (event.type == SDL_CONTROLLERAXISMOTION) {
                // ignore analog sticks
            } else if (event.type == SDL_CONTROLLERBUTTONDOWN) {
                output.buttonsPressed |= bits;
                _buttonsHeld |= bits;
            } else {
                output.buttonsReleased |= bits;
                _buttonsHeld &= ~bits;
            }

            output.timestamp = event.cbutton.timestamp;
            output.buttonsHeld = _buttonsHeld;
            valid = true;
        }
    }

    return valid;
#endif
}

}
