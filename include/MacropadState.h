#pragma once

#include <stdint.h>
#include "SH1106_SPI.h"

typedef void (*oledDraw)(SH1106_SPI oled);
typedef void (*keyCallback)(bool, bool);
typedef void (*keyCallbackGeneric)(uint8_t, bool, bool);
typedef void (*encoderCallback)(int, int);
typedef void (*encoderPressedCallback)(bool, bool);

class MacropadState
{
public:
	MacropadState(MacropadState* parent, keyCallbackGeneric all_keys = nullptr, keyCallback key_one = nullptr, keyCallback key_two = nullptr, keyCallback key_three = nullptr, keyCallback key_four = nullptr, keyCallback key_five = nullptr, keyCallback key_six = nullptr, keyCallback key_seven = nullptr, keyCallback key_eight = nullptr, keyCallback key_nine = nullptr, keyCallback key_ten = nullptr, keyCallback key_eleven = nullptr, keyCallback key_twelve = nullptr, encoderCallback encoder_callback = nullptr, encoderPressedCallback encoder_pressed = nullptr, oledDraw oled_draw = nullptr);
	MacropadState(keyCallbackGeneric all_keys = nullptr, keyCallback key_one = nullptr, keyCallback key_two = nullptr, keyCallback key_three = nullptr, keyCallback key_four = nullptr, keyCallback key_five = nullptr, keyCallback key_six = nullptr, keyCallback key_seven = nullptr, keyCallback key_eight = nullptr, keyCallback key_nine = nullptr, keyCallback key_ten = nullptr, keyCallback key_eleven = nullptr, keyCallback key_twelve = nullptr, encoderCallback encoder_callback = nullptr, encoderPressedCallback encoder_pressed = nullptr, oledDraw oled_draw = nullptr);
	void set_pixel_color(uint8_t pixel, uint32_t color);
	void set_pixels_brightness(uint16_t brightness);
	void set_oled_draw_function(oledDraw newFunc);
	void set_encoder_position(int newPos);
	void set_encoder_pressed_callback(encoderPressedCallback newFunc);
	void set_encoder_callback(encoderCallback newFunc);
	void set_key_callback(uint8_t key, keyCallback newFunc);
	void set_key_generic_callback(keyCallbackGeneric newFunc);
	void set_oled_automatic_updates(bool enable = true);
	void set_oled_automatic_updates(bool on_encoder_move, bool on_encoder_press, bool on_key_press);
	void set_parent_state(MacropadState* parent);
	int get_encoder_position();
	int get_encoder_last_postition();
	bool get_update_on_encoder_move();
	bool get_update_on_encoder_press();
	bool get_update_on_key_press();
	uint16_t get_pixels_brightness();
	uint32_t get_pixel_color(uint8_t pixel);
	oledDraw get_oled_draw_function();
	keyCallback get_key_callback(uint8_t key);
	keyCallbackGeneric get_key_generic_callback();
	encoderCallback get_encoder_callback();
	encoderPressedCallback get_encoder_pressed_callback();
	MacropadState* get_parent_state();
private:
	int m_encoderPosition = 0;
	int m_encoderLastPosition = -1;
	bool m_updateOnKeyPress = true;
	bool m_updateOnEncoderMove = true;
	bool m_updateOnEncoderPress = true;
    int16_t m_brightness = 80;
	uint32_t m_colors[12] = {0};
	oledDraw m_oled_draw_function = nullptr;
    keyCallback m_keyPressedCallbacks[12] = {nullptr};
    keyCallbackGeneric m_keyPressedGeneric = nullptr;
	encoderCallback m_encoderCallback = nullptr;
	encoderPressedCallback m_encoderPressedCallback = nullptr;
	MacropadState* m_parent = nullptr;
};
