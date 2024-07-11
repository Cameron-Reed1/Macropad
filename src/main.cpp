#include "main.h"
#include "Config.h"
#include "RippleAnim.h"
#include "LinkMacro.h"
#include <pico/bootrom.h>

void save_conf(bool rising, bool falling);
void erase_conf(bool rising, bool falling);
MacropadState default_state(button_any, load_teams_state, load_generic_state, load_brightness_adjustment_state, build_link_macro, load_dev_state, nullptr, nullptr, nullptr, nullptr, erase_conf, save_conf, /*load_dummy_state*/ nullptr, encoder_handler, encoder_pressed, oled_draw);
MacropadState genericState;
Macropad& macropad = Macropad::get_instance();

uint64_t press_start[12] = {0};

int main(void) {
    CFG* cfg = Config::read();
	macropad.init(&default_state, true, true, true, false, false, true, false);
	ripple_anim_setup();
    ripple_anim_set_brightness(cfg->brightness);

	while (true) {
		macropad.run();
	}

	return 0;
}

void button_any(uint8_t key, bool rising, bool falling) {
	// uint16_t colorIndex = (get_absolute_time() / 1000000) % 765;

	// uint8_t red = ((255 - colorIndex) * (colorIndex < 255)) + ((colorIndex - 510) * (colorIndex >= 510));
	// uint8_t green = ((colorIndex) * (colorIndex < 255)) + ((510 - colorIndex) * (colorIndex >= 255 && colorIndex < 510));
	// uint8_t blue = ((colorIndex - 255) * (colorIndex >= 255 && colorIndex < 510)) + ((765 - colorIndex) * (colorIndex >= 510));

    CFG* cfg = Config::get();

	if (rising) {
        // macropad.set_pixel_color(key, cfg->colorOutR, cfg->colorOutG, cfg->colorOutB);
		press_start[key] = get_absolute_time();
		ripple_anim_run(key, cfg->colorOutR, cfg->colorOutG, cfg->colorOutB, DIR_OUT);
	} else if (falling) {
        // macropad.set_pixel_color(key, 0, 0, 0);
		if (get_absolute_time() - press_start[key] > 320 * 1000)
			ripple_anim_run(key, cfg->colorInR, cfg->colorInG, cfg->colorInB, DIR_IN);
	}
}

void erase_conf(bool rising, bool falling)
{
    (void) rising;

    if (falling) {
        Config::erase();
    }
}

void save_conf(bool rising, bool falling)
{
    (void) rising;

    if (falling) {
        Config::flush();
    }
}

void return_to_parent_state(bool rising, bool falling)
{
	(void) rising;

	if (falling)
        macropad.load_parent_state();
}

void load_teams_state(bool rising, bool falling)
{
	(void) rising;
	static MacropadState teamsState =
		MacropadState(&default_state, button_any,
					return_to_parent_state, nullptr, nullptr,
					nullptr, nullptr, nullptr,
					teams_blur, press_keys<HID_KEY_CONTROL_LEFT, HID_KEY_SHIFT_LEFT, HID_KEY_H>, press_keys<HID_KEY_CONTROL_LEFT, HID_KEY_SHIFT_LEFT, HID_KEY_K>,
					press_keys<HID_KEY_CONTROL_LEFT, HID_KEY_SPACE>, press_keys<HID_KEY_CONTROL_LEFT, HID_KEY_SHIFT_LEFT, HID_KEY_M>, press_keys<HID_KEY_CONTROL_LEFT, HID_KEY_SHIFT_LEFT, HID_KEY_O>,
					encoder_handler, encoder_pressed, oled_draw_teams_state);
	teamsState.set_oled_automatic_updates(false);

	if (falling)
        macropad.set_macropad_state(&teamsState);
}

void load_generic_state(bool rising, bool falling)
{
	(void) rising;
	genericState = MacropadState(&default_state, button_any,
									return_to_parent_state, press_keys<HID_KEY_GUI_LEFT, HID_KEY_L>, press_keys<HID_KEY_GUI_LEFT, HID_KEY_D>,
									press_keys<HID_KEY_CONTROL_LEFT, HID_KEY_C>, press_keys<HID_KEY_CONTROL_LEFT, HID_KEY_V>, press_keys<HID_KEY_CONTROL_LEFT, HID_KEY_A>,
									press_keys<HID_KEY_CONTROL_LEFT, HID_KEY_SHIFT_LEFT, HID_KEY_ESCAPE>, press_keys<HID_KEY_GUI_LEFT, HID_KEY_I>, press_keys<HID_KEY_GUI_LEFT, HID_KEY_SHIFT_LEFT, HID_KEY_S>,
									type_hello_world, type_monitor, power_off,
									encoder_handler, encoder_pressed, oled_draw_generic_state);
	genericState.set_oled_automatic_updates(false);

	if (falling)
        macropad.set_macropad_state(&genericState);
}

void load_brightness_adjustment_state(bool rising, bool falling)
{
	static MacropadState brightnessAdjustState(&default_state, button_any,
									nullptr, nullptr, load_brightness_adjustment_state,
									nullptr, nullptr, nullptr,
									nullptr, nullptr, nullptr,
									nullptr, nullptr, nullptr,
									encoder_handler_brightness_adjust, nullptr, oled_draw_brightness_adjust_state);

//	static bool set_initial_position = false;
//	if (!set_initial_position) {
//		set_initial_position = true;
//		brightnessAdjustState.set_encoder_position(80);
//	}

    CFG* cfg = Config::get();

	if (rising) {
        brightnessAdjustState.set_encoder_position(cfg->brightness);
		macropad.set_macropad_state(&brightnessAdjustState);
	}

	if (falling) {
        macropad.set_pixels_brightness(macropad.get_encoder_position());
		ripple_anim_set_brightness(macropad.get_encoder_position());
        cfg->brightness = macropad.get_encoder_position();
		macropad.load_parent_state();
        // Config::flush();
	}
}

void load_dev_state(bool rising, bool falling)
{
	(void) rising;
	static MacropadState devState =
		MacropadState(&default_state, button_any,
					return_to_parent_state, enter_bootloader, nullptr,
					nullptr, nullptr, nullptr,
					nullptr, nullptr, nullptr,
					nullptr, nullptr, nullptr,
					nullptr, nullptr, oled_draw_dev_state);
	devState.set_oled_automatic_updates(false);

	if (falling)
        macropad.set_macropad_state(&devState);
}

void type_hello_world(bool rising, bool falling)
{
	(void) falling;

	if (rising)
        macropad.type("Hello, World!");
}

void type_monitor(bool rising, bool falling)
{
	(void) falling;

	if (rising)
        macropad.type("Monitor");
}

void power_off(bool rising, bool falling) {
	(void) rising;
	static MacropadState powerState(&genericState, button_any,
									return_to_parent_state, return_to_parent_state, return_to_parent_state,
									return_to_parent_state, return_to_parent_state, return_to_parent_state,
									return_to_parent_state, return_to_parent_state, return_to_parent_state,
									return_to_parent_state, return_to_parent_state, [](bool rising, bool falling) {
										if (rising)
											Macropad::get_instance().press_system_key(HID_USAGE_DESKTOP_SYSTEM_POWER_DOWN);
										else if (falling) {
											Macropad::get_instance().release_system_keys();
											Macropad::get_instance().load_parent_state();
										}
									},
									nullptr, nullptr, oled_draw_power_state);
	powerState.set_oled_automatic_updates(false);

	if (falling)
        macropad.set_macropad_state(&powerState);
}

void teams_blur(bool rising, bool falling)
{
	(void) falling;

	static Macro* macro = nullptr;
	if (macro == nullptr) {
		macro = new Macro(6);
		macro->addKeyPressStep(HID_KEY_CONTROL_LEFT, 1, HID_KEY_SHIFT_LEFT, HID_KEY_P).addSleepStep(500).addKeyPressStep(HID_KEY_ENTER).addKeyPressStep(HID_KEY_TAB, 2).addKeyPressStep(HID_KEY_ENTER);
	}

	if (rising)
		macropad.run_macro(macro);

}

void enter_bootloader(bool rising, bool falling)
{
	(void) rising;
	(void) falling;

	reset_usb_boot(1 << LED, 0);
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

	if (rising)
		macropad.run_macro(macro);

}

void encoder_handler(int last_position, int new_position)
{
	static bool key_pressed = false;
	if (new_position - last_position > 0) {
		macropad.press_consumer_key(HID_USAGE_CONSUMER_VOLUME_INCREMENT);
		key_pressed = true;
	} else if (new_position - last_position < 0) {
		macropad.press_consumer_key(HID_USAGE_CONSUMER_VOLUME_DECREMENT);
		key_pressed = true;
	} else if (key_pressed) {
		macropad.release_consumer_keys();
		key_pressed = false;
	}
}

void encoder_handler_brightness_adjust(int last_position, int new_position) {
	(void) last_position;

	if (new_position < 0) {
	    macropad.set_encoder_position(0);
	} else if (new_position > 255) {
	    macropad.set_encoder_position(255);
	}
    
    Config::get()->brightness = macropad.get_encoder_position();
}

void encoder_pressed(bool rising, bool falling)
{
	if (rising) {
		macropad.press_consumer_key(HID_USAGE_CONSUMER_MUTE);
	} else if (falling) {
		macropad.release_consumer_keys();
	}
}

void oled_draw(SH1106_SPI oled)
{
	oled.gotoXY(0, 0);
	oled.print("Encoder Position: ");
	oled.print(macropad.get_encoder_position());

    CFG* cfg = Config::get();
    oled.gotoXY(0, 1);
    oled.print("Config value: ");
    oled.print(cfg->brightness);

    CFG* cfg_saved = Config::savedValues();
    oled.gotoXY(0, 2);
    oled.print("Saved value: ");
    oled.print(cfg_saved->brightness);

	for (int i = NUM_KEYS; i < NUM_KEYS; i++) {
		if ( macropad.get_key_state(i + 1)) {
			oled.gotoXY((i % 3) * 48, (i / 3) + 4);
			oled.print("KEY");
			oled.print(i + 1);
		}
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

void oled_draw_dev_state(SH1106_SPI oled)
{
	const char* const functions[4][3] = {
		{"Back", "Boot", ""},
		{"", "", ""},
		{"", "", ""},
		{"", "", ""}
	};

	for (uint8_t y = 0; y < 4; y++) {
		for (uint8_t x = 0; x < 3; x++) {
			oled.gotoXY(x * 45, y * 2);
			oled.print(functions[y][x]);
		}
	}
}

void oled_draw_generic_state(SH1106_SPI oled)
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

void oled_draw_power_state ( SH1106_SPI oled )
{
	oled.gotoXY(0, 0);
	oled.print("Are you sure?");
	oled.gotoXY(86, 5);
	oled.print("Yes");
}


void oled_draw_brightness_adjust_state(SH1106_SPI oled) {
	oled.gotoXY(0, 0);
	oled.print("Brightness: ");
	oled.print(macropad.get_encoder_position());

    CFG* cfg = Config::get();
    oled.gotoXY(0, 1);
    oled.print("Config value: ");
    oled.print(cfg->brightness);

	for (int i = 0; i < NUM_KEYS; i++) {
		if ( macropad.get_key_state(i + 1)) {
			oled.gotoXY((i % 3) * 48, (i / 3) + 4);
			oled.print("KEY");
			oled.print(i + 1);
		}
	}
}
