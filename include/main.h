#pragma once

#include "Macropad.h"

void button_any(uint8_t key, bool rising, bool falling);
void return_to_parent_state(bool rising, bool falling);

void load_teams_state(bool rising, bool falling);
void load_generic_state(bool rising, bool falling);
void load_brightness_adjustment_state(bool rising, bool falling);
void load_dev_state(bool rising, bool falling);

template <uint8_t key>
void press_key(bool rising, bool falling) {
	if (rising)
		Macropad::get_instance().press_key(key);
	else if (falling)
		Macropad::get_instance().release_key(key);
}

template <uint8_t key1, uint8_t key2>
void press_keys(bool rising, bool falling) {
	if (rising) {
		Macropad::get_instance().press_key(key1);
		Macropad::get_instance().press_key(key2);
	} else if (falling) {
		Macropad::get_instance().release_key(key1);
		Macropad::get_instance().release_key(key2);
	}
}

template <uint8_t key1, uint8_t key2, uint8_t key3>
void press_keys(bool rising, bool falling) {
	if (rising) {
		Macropad::get_instance().press_key(key1);
		Macropad::get_instance().press_key(key2);
		Macropad::get_instance().press_key(key3);
	} else if (falling) {
		Macropad::get_instance().release_key(key1);
		Macropad::get_instance().release_key(key2);
		Macropad::get_instance().release_key(key3);
	}
}

template <uint8_t key>
void press_consumer_key(bool rising, bool falling)
{
	if (rising)
		Macropad::get_instance().press_consumer_key(key);
	else if (falling)
		Macropad::get_instance().release_consumer_keys();
}

template <uint8_t key>
void press_system_key(bool rising, bool falling)
{
	if (rising)
		Macropad::get_instance().press_system_key(key);
	else if (falling)
		Macropad::get_instance().release_system_keys();
}

template <const char* str>
void type_a(bool rising, bool falling)
{
	(void) falling;

	if (rising)
        Macropad::get_instance().type(str);
}

void type_hello_world(bool rising, bool falling);
void type_monitor(bool rising, bool falling);
void dock_macro(bool rising, bool falling);
void teams_blur(bool rising, bool falling);
void power_off(bool rising, bool falling);
void enter_bootloader(bool rising, bool falling);

void encoder_handler(int last_position, int new_position);
void encoder_pressed(bool rising, bool falling);

void oled_draw(SH1106_SPI oled);
void oled_draw_teams_state(SH1106_SPI oled);
void oled_draw_generic_state(SH1106_SPI oled);
void oled_draw_power_state(SH1106_SPI oled);
void oled_draw_dev_state(SH1106_SPI oled);
void oled_draw_brightness_adjust_state(SH1106_SPI oled);

void encoder_handler_brightness_adjust(int last_position, int new_position);
