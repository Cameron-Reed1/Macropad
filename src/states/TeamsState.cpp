#include "states/TeamsState.h"
#include "states/common.h"
#include "MacropadState.h"
#include "Macropad.h"


namespace TeamsState {

void blur(bool rising, bool falling);

void encoder_handler(int last_position, int new_position);
void encoder_pressed(bool rising, bool falling);
void oled_draw_teams_state(SH1106_SPI oled);

static MacropadState teamsState(ripple,
        return_to_parent_state, nullptr, nullptr,
        nullptr, nullptr, nullptr,
        blur, press_keys<HID_KEY_CONTROL_LEFT, HID_KEY_SHIFT_LEFT, HID_KEY_H>, press_keys<HID_KEY_CONTROL_LEFT, HID_KEY_SHIFT_LEFT, HID_KEY_K>,
        press_keys<HID_KEY_CONTROL_LEFT, HID_KEY_SPACE>, press_keys<HID_KEY_CONTROL_LEFT, HID_KEY_SHIFT_LEFT, HID_KEY_M>, press_keys<HID_KEY_CONTROL_LEFT, HID_KEY_SHIFT_LEFT, HID_KEY_O>,
        encoder_volume, toggle_mute, oled_draw_teams_state);

void load_state(MacropadState* parent)
{
    teamsState.set_parent_state(parent);
    teamsState.set_oled_automatic_updates(false);
    Macropad::get_instance().set_macropad_state(&teamsState);
}

void blur(bool rising, bool falling)
{
	(void) falling;

	static Macro* macro = nullptr;
	if (macro == nullptr) {
		macro = new Macro(6);
		macro->addKeyPressStep(HID_KEY_CONTROL_LEFT, 1, HID_KEY_SHIFT_LEFT, HID_KEY_P).addSleepStep(500).addKeyPressStep(HID_KEY_ENTER).addKeyPressStep(HID_KEY_TAB, 2).addKeyPressStep(HID_KEY_ENTER);
	}

	if (rising) {
        Macropad::get_instance().run_macro(macro);
    }
}

void encoder_handler(int last_position, int new_position)
{
	static bool key_pressed = false;
	if (new_position - last_position > 0) {
        Macropad::get_instance().press_consumer_key(HID_USAGE_CONSUMER_VOLUME_INCREMENT);
		key_pressed = true;
	} else if (new_position - last_position < 0) {
		Macropad::get_instance().press_consumer_key(HID_USAGE_CONSUMER_VOLUME_DECREMENT);
		key_pressed = true;
	} else if (key_pressed) {
        Macropad::get_instance().release_consumer_keys();
		key_pressed = false;
	}
}

void encoder_pressed(bool rising, bool falling)
{
	if (rising) {
        Macropad::get_instance().press_consumer_key(HID_USAGE_CONSUMER_MUTE);
	} else if (falling) {
        Macropad::get_instance().release_consumer_keys();
	}
}

void oled_draw_teams_state(SH1106_SPI oled)
{
	const char* const functions[4][3] = {
		{"Back", "", ""},
		{"", "", ""},
		{"Blur", "Leave", "Raise"},
		{"Talk", "Mute", "Camera"}
	};

	for (uint8_t y = 0; y < 4; y++) {
		for (uint8_t x = 0; x < 3; x++) {
			oled.gotoXY(x * 45, y * 2);
			oled.print(functions[y][x]);
		}
	}
}

}

