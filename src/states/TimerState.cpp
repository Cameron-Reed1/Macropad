#include "states/TimerState.h"
#include "class/hid/hid.h"
#include "states/common.h"
#include "MacropadState.h"
#include "Macropad.h"


namespace TimerState {

void oled_draw(SH1106_SPI oled);

static MacropadState timerState(ripple,
        return_to_parent_state, press_keys<HID_KEY_ALT_LEFT, HID_KEY_F5>, press_keys<HID_KEY_ALT_LEFT, HID_KEY_F6>,
        press_keys<HID_KEY_ALT_LEFT, HID_KEY_F7>, press_keys<HID_KEY_ALT_LEFT, HID_KEY_F8>, press_keys<HID_KEY_ALT_LEFT, HID_KEY_F9>,
        press_keys<HID_KEY_CONTROL_LEFT, HID_KEY_F5>, press_keys<HID_KEY_CONTROL_LEFT, HID_KEY_F7>, press_keys<HID_KEY_CONTROL_LEFT, HID_KEY_F8>,
        press_keys<HID_KEY_CONTROL_LEFT, HID_KEY_F6>, nullptr, nullptr,
        nullptr, nullptr, oled_draw);


void load_state(MacropadState* parent)
{
    timerState.set_parent_state(parent);
    timerState.set_oled_automatic_updates(false);

    Macropad::get_instance().set_macropad_state(&timerState);
}


void oled_draw(SH1106_SPI oled)
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

} // namespace TimerState

