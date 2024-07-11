#include "states/PowerOffState.h"
#include "states/GenericState.h"
#include "states/common.h"
#include "MacropadState.h"
#include "Macropad.h"


namespace GenericState {

void type_hello_world(bool rising, bool falling);
void type_monitor(bool rising, bool falling);
void confirm_poweroff(bool rising, bool falling);
void dock_macro(bool rising, bool falling);

void oled_draw(SH1106_SPI oled);

static MacropadState genericState(ripple,
            return_to_parent_state, press_keys<HID_KEY_GUI_LEFT, HID_KEY_L>, press_keys<HID_KEY_GUI_LEFT, HID_KEY_D>,
            press_keys<HID_KEY_CONTROL_LEFT, HID_KEY_C>, press_keys<HID_KEY_CONTROL_LEFT, HID_KEY_V>, press_keys<HID_KEY_CONTROL_LEFT, HID_KEY_A>,
            press_keys<HID_KEY_CONTROL_LEFT, HID_KEY_SHIFT_LEFT, HID_KEY_ESCAPE>, press_keys<HID_KEY_GUI_LEFT, HID_KEY_I>, press_keys<HID_KEY_GUI_LEFT, HID_KEY_SHIFT_LEFT, HID_KEY_S>,
            type_hello_world, type_monitor, confirm_poweroff,
            encoder_volume, toggle_mute, oled_draw);

void load_state(MacropadState *parent)
{
    genericState.set_parent_state(parent);
    genericState.set_oled_automatic_updates(false);
    Macropad::get_instance().set_macropad_state(&genericState);
}

void type_hello_world(bool rising, bool falling)
{
	(void) falling;

	if (rising) {
        Macropad::get_instance().type("Hello, World!");
    }
}

void type_monitor(bool rising, bool falling)
{
	(void) falling;

	if (rising) {
        Macropad::get_instance().type("Monitor");
    }
}

void dock_macro(bool rising, bool falling)
{
	(void) falling;

	static Macro* macro = nullptr;
	if (macro == nullptr) {
		macro = new Macro(33);
		macro->addKeyPressStep(HID_KEY_ENTER).addSleepStep(1500).addKeyPressStep(HID_KEY_TAB, 4)
		.addKeyPressStep(HID_KEY_ENTER).addSleepStep(250).addKeyPressStep(HID_KEY_ARROW_DOWN)
		.addKeyPressStep(HID_KEY_ENTER, 2).addSleepStep(1500).addKeyPressStep(HID_KEY_TAB, 5)
		.addKeyPressStep(HID_KEY_ENTER).addSleepStep(1000).addKeyPressStep(HID_KEY_SHIFT_LEFT, 1, HID_KEY_END)
		.addKeyPressStep(HID_KEY_BACKSPACE).addKeyPressStep(HID_KEY_TAB).addKeyPressStep(HID_KEY_ENTER).addSleepStep(1500)
		.addKeyPressStep(HID_KEY_TAB, 6).addKeyPressStep(HID_KEY_ENTER).addSleepStep(1500)
		.addKeyPressStep(HID_KEY_ARROW_DOWN);
	}

	if (rising) {
        Macropad::get_instance().run_macro(macro);
    }
}

void confirm_poweroff(bool rising, bool falling)
{
	(void) rising;

	if (falling) {
        PowerOffState::load_state(Macropad::get_instance().get_macropad_state());
    }
}

void oled_draw(SH1106_SPI oled)
{
	const char* const functions[8][3] = {
		{"Back", "Lock", "Desktop"},
		{"","",""},
		{"Copy", "Paste", "Select"},
		{"","",""},
		{"Task", "Setting", "Screen"},
		{"Manager","","Shot"},
		{"Hello", "Monitor", "Power"},
		{"","",""}
	};

	for (uint8_t y = 0; y < 8; y++) {
		for (uint8_t x = 0; x < 3; x++) {
			oled.gotoXY(x * 43, y);
			oled.print(functions[y][x]);
		}
	}
}

}
