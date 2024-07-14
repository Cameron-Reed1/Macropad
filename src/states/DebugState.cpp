#include <pico/bootrom.h>
#include <stdint.h>

#include "states/DebugState.h"
#include "states/common.h"
#include "MacropadState.h"
#include "PinDefs.h"
#include "Config.h"


DebugState::DebugState(MacropadState* parent)
    : MacropadState(parent) { }


void DebugState::KeyAny(uint8_t key, bool rising, bool falling)
{
    ripple(key, rising, falling);
}

void DebugState::Key1(bool rising, bool falling)
{
    return_to_parent_state(rising, falling);
}

void DebugState::Key2(bool rising, bool falling)
{
    (void) rising;
    (void) falling;

    reset_usb_boot(1 << LED, 0);
}

void DebugState::Key3(bool rising, bool falling)
{
    (void) falling;

    if (rising) {
        Config::eraseAll();
    }
}


void DebugState::OledDraw(SH1106_SPI oled)
{
    const char* const labels[4][3] = {
        {"Back", "Boot", "Erase"},
        {"", "", ""},
        {"", "", ""},
        {"", "", ""}
    };

    for (uint8_t y = 0; y < 4; y++) {
        for (uint8_t x = 0; x < 3; x++) {
            oled.gotoXY(x * 45, y * 2);
            oled.print(labels[y][x]);
        }
    }
}

