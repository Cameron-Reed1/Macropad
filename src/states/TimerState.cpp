#include "states/TimerState.h"
#include "class/hid/hid.h"
#include "states/common.h"
#include "MacropadState.h"


TimerState::TimerState(MacropadState* parent)
    : MacropadState(parent) { }

void TimerState::KeyAny(uint8_t key, bool rising, bool falling)
{
    ripple(key, rising, falling);
}

void TimerState::Key1(bool rising, bool falling)
{
    return_to_parent_state(rising, falling);
}

void TimerState::Key2(bool rising, bool falling)
{
    press_keys<HID_KEY_ALT_LEFT, HID_KEY_F5>(rising, falling);
}

void TimerState::Key3(bool rising, bool falling)
{
    press_keys<HID_KEY_ALT_LEFT, HID_KEY_F6>(rising, falling);
}

void TimerState::Key4(bool rising, bool falling)
{
    press_keys<HID_KEY_ALT_LEFT, HID_KEY_F7>(rising, falling);
}

void TimerState::Key5(bool rising, bool falling)
{
    press_keys<HID_KEY_ALT_LEFT, HID_KEY_F8>(rising, falling);
}

void TimerState::Key6(bool rising, bool falling)
{
    press_keys<HID_KEY_ALT_LEFT, HID_KEY_F9>(rising, falling);
}

void TimerState::Key7(bool rising, bool falling)
{
    press_keys<HID_KEY_CONTROL_LEFT, HID_KEY_F5>(rising, falling);
}

void TimerState::Key8(bool rising, bool falling)
{
    press_keys<HID_KEY_CONTROL_LEFT, HID_KEY_F7>(rising, falling);
}

void TimerState::Key9(bool rising, bool falling)
{
    press_keys<HID_KEY_CONTROL_LEFT, HID_KEY_F8>(rising, falling);
}

void TimerState::Key10(bool rising, bool falling)
{
    press_keys<HID_KEY_CONTROL_LEFT, HID_KEY_F6>(rising, falling);
}


void TimerState::OledDraw(SH1106_SPI oled)
{
    const char* const labels[4][3] = {
        {"Back", "1min", "5min"},
        {"15min", "1hr", "Clear"},
        {"Start", "Pause", "Resume"},
        {"Clear", "", ""},
    };

    for (uint8_t y = 0; y < 4; y++) {
        for (uint8_t x = 0; x < 3; x++) {
            oled.gotoXY(x * 43, y * 2);
            oled.print(labels[y][x]);
        }
    }
}

