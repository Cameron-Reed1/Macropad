#pragma once

#include <stdint.h>

#include "Macropad.h"


void ripple(uint8_t key, bool rising, bool falling);
void return_to_parent_state(bool rising, bool falling);
void encoder_volume(int last_position, int new_position);
void encoder_brightness(int last_position, int new_position);
void toggle_mute(bool rising, bool falling);


template <uint8_t key>
void press_key(bool rising, bool falling) {
    if (rising) {
        macropad::PressKey(key);
    } else if (falling) {
        macropad::ReleaseKey(key);
    }
}

template <uint8_t key1, uint8_t key2>
void press_keys(bool rising, bool falling) {
    if (rising) {
        macropad::PressKey(key1);
        macropad::PressKey(key2);
    } else if (falling) {
        macropad::ReleaseKey(key1);
        macropad::ReleaseKey(key2);
    }
}

template <uint8_t key1, uint8_t key2, uint8_t key3>
void press_keys(bool rising, bool falling) {
    if (rising) {
        macropad::PressKey(key1);
        macropad::PressKey(key2);
        macropad::PressKey(key3);
    } else if (falling) {
        macropad::ReleaseKey(key1);
        macropad::ReleaseKey(key2);
        macropad::ReleaseKey(key3);
    }
}

template <uint16_t key>
void press_consumer_key(bool rising, bool falling)
{
    if (rising) {
        macropad::PressConsumerKey(key);
    } else if (falling) {
        macropad::ReleaseConsumerKeys();
    }
}

template <uint16_t key>
void press_system_key(bool rising, bool falling)
{
    if (rising) {
        macropad::PressSystemKey(key);
    } else if (falling) {
        macropad::ReleaseSystemKeys();
    }
}

template <const char* str>
void type(bool rising, bool falling)
{
    (void) falling;

    if (rising) {
        macropad::Type(str);
    }
}

