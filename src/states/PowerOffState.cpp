#include "states/PowerOffState.h"
#include "states/common.h"
#include "MacropadState.h"
#include "Macropad.h"


PowerOffState::PowerOffState(MacropadState* parent)
    : MacropadState(parent) { }


void PowerOffState::KeyAny(uint8_t key, bool rising, bool falling)
{
    if (key != 12) {
        return_to_parent_state(rising, falling);
    }

    ripple(key, rising, falling);
}

void PowerOffState::Key12(bool rising, bool falling)
{
    Macropad& macropad = Macropad::get_instance();
    if (rising) {
        macropad.press_system_key(HID_USAGE_DESKTOP_SYSTEM_POWER_DOWN);
    } else if (falling) {
        macropad.release_system_keys();
        macropad.load_parent_state();
    }
}


void PowerOffState::OledDraw(SH1106_SPI oled)
{
    oled.gotoXY(0, 0);
    oled.print("Are you sure?");

    oled.gotoXY(86, 5);
    oled.print("Yes");
}

