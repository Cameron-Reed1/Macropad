#include "states/PowerOffState.h"
#include "states/common.h"
#include "MacropadState.h"
#include "Macropad.h"


namespace PowerOffState {

void poweroff(bool rising, bool falling);

void oled_draw(SH1106_SPI oled);

static MacropadState powerOffState(ripple,
        return_to_parent_state, return_to_parent_state, return_to_parent_state,
        return_to_parent_state, return_to_parent_state, return_to_parent_state,
        return_to_parent_state, return_to_parent_state, return_to_parent_state,
        return_to_parent_state, return_to_parent_state, poweroff,
        nullptr, nullptr, oled_draw);

void load_state(MacropadState* parent)
{
    powerOffState.set_parent_state(parent);
    powerOffState.set_oled_automatic_updates(false);
    Macropad::get_instance().set_macropad_state(&powerOffState);
}

void poweroff(bool rising, bool falling)
{
    if (rising) {
        Macropad::get_instance().press_system_key(HID_USAGE_DESKTOP_SYSTEM_POWER_DOWN);
    } else if (falling) {
        Macropad::get_instance().release_system_keys();
        Macropad::get_instance().load_parent_state();
    }
}

void oled_draw(SH1106_SPI oled)
{
	oled.gotoXY(0, 0);
	oled.print("Are you sure?");

	oled.gotoXY(86, 5);
	oled.print("Yes");
}

}

