#pragma once

#include <stdint.h>

#include "Macropad.h"


void ripple(uint8_t key, bool rising, bool falling);
void return_to_parent_state(bool rising, bool falling);
void encoder_volume(int last_position, int new_position);
void toggle_mute(bool rising, bool falling);


template <uint8_t key>
void press_key(bool rising, bool falling) {
	if (rising) {
		Macropad::get_instance().press_key(key);
    } else if (falling) {
		Macropad::get_instance().release_key(key);
    }
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
	if (rising) {
		Macropad::get_instance().press_consumer_key(key);
    } else if (falling) {
		Macropad::get_instance().release_consumer_keys();
    }
}

template <uint8_t key>
void press_system_key(bool rising, bool falling)
{
	if (rising) {
        Macropad::get_instance().press_system_key(key);
    } else if (falling) {
        Macropad::get_instance().release_system_keys();
    }
}

template <const char* str>
void type(bool rising, bool falling)
{
	(void) falling;

	if (rising) {
        Macropad::get_instance().type(str);
    }
}

