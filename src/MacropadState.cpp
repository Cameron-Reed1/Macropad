#include "MacropadState.h"

MacropadState::MacropadState ( MacropadState* parent, keyCallbackGeneric all_keys, keyCallback key_one, keyCallback key_two, keyCallback key_three, keyCallback key_four, keyCallback key_five, keyCallback key_six, keyCallback key_seven, keyCallback key_eight, keyCallback key_nine, keyCallback key_ten, keyCallback key_eleven, keyCallback key_twelve, encoderCallback encoder_callback, encoderPressedCallback encoder_pressed, oledDraw oled_draw ) : m_oled_draw_function(oled_draw), m_keyPressedGeneric(all_keys), m_encoderCallback(encoder_callback), m_encoderPressedCallback(encoder_pressed), m_parent(parent)
{
	m_keyPressedCallbacks[0] = key_one;
	m_keyPressedCallbacks[1] = key_two;
	m_keyPressedCallbacks[2] = key_three;
	m_keyPressedCallbacks[3] = key_four;
	m_keyPressedCallbacks[4] = key_five;
	m_keyPressedCallbacks[5] = key_six;
	m_keyPressedCallbacks[6] = key_seven;
	m_keyPressedCallbacks[7] = key_eight;
	m_keyPressedCallbacks[8] = key_nine;
	m_keyPressedCallbacks[9] = key_ten;
	m_keyPressedCallbacks[10] = key_eleven;
	m_keyPressedCallbacks[11] = key_twelve;
}


MacropadState::MacropadState(keyCallbackGeneric all_keys, keyCallback key_one, keyCallback key_two, keyCallback key_three, keyCallback key_four, keyCallback key_five, keyCallback key_six, keyCallback key_seven, keyCallback key_eight, keyCallback key_nine, keyCallback key_ten, keyCallback key_eleven, keyCallback key_twelve, encoderCallback encoder_callback, encoderPressedCallback encoder_pressed, oledDraw oled_draw) : m_oled_draw_function(oled_draw), m_keyPressedGeneric(all_keys), m_encoderCallback(encoder_callback), m_encoderPressedCallback(encoder_pressed)
{
	m_keyPressedCallbacks[0] = key_one;
	m_keyPressedCallbacks[1] = key_two;
	m_keyPressedCallbacks[2] = key_three;
	m_keyPressedCallbacks[3] = key_four;
	m_keyPressedCallbacks[4] = key_five;
	m_keyPressedCallbacks[5] = key_six;
	m_keyPressedCallbacks[6] = key_seven;
	m_keyPressedCallbacks[7] = key_eight;
	m_keyPressedCallbacks[8] = key_nine;
	m_keyPressedCallbacks[9] = key_ten;
	m_keyPressedCallbacks[10] = key_eleven;
	m_keyPressedCallbacks[11] = key_twelve;
}

MacropadState* MacropadState::get_parent_state()
{
	return m_parent;
}

void MacropadState::set_parent_state(MacropadState* parent)
{
	m_parent = parent;
}

void MacropadState::set_oled_automatic_updates(bool enable /* = true */)
{
	this->set_oled_automatic_updates(enable, enable, enable);
}

void MacropadState::set_oled_automatic_updates(bool on_encoder_move, bool on_encoder_press, bool on_key_press)
{
	m_updateOnEncoderMove = on_encoder_move;
	m_updateOnEncoderPress = on_encoder_press;
	m_updateOnKeyPress = on_key_press;
}

void MacropadState::set_pixels_brightness(uint16_t brightness)
{
	m_brightness = brightness;
}

void MacropadState::set_pixel_color(uint8_t pixel, uint32_t color)
{
    m_colors[pixel] = color;
}

void MacropadState::set_oled_draw_function(oledDraw newFunc)
{
	m_oled_draw_function = newFunc;
}

void MacropadState::set_key_callback(uint8_t key, keyCallback newFunc)
{
	m_keyPressedCallbacks[key] = newFunc;
}

void MacropadState::set_key_generic_callback(keyCallbackGeneric newFunc)
{
	m_keyPressedGeneric = newFunc;
}

void MacropadState::set_encoder_position(int newPos)
{
	m_encoderLastPosition = m_encoderPosition;
	m_encoderPosition = newPos;
}

void MacropadState::set_encoder_callback(encoderCallback newFunc)
{
	m_encoderCallback = newFunc;
}

void MacropadState::set_encoder_pressed_callback(encoderPressedCallback newFunc)
{
	m_encoderPressedCallback = newFunc;
}

uint16_t MacropadState::get_pixels_brightness()
{
	return m_brightness;
}

uint32_t MacropadState::get_pixel_color(uint8_t pixel)
{
	return m_colors[pixel];
}

oledDraw MacropadState::get_oled_draw_function()
{
	return m_oled_draw_function;
}

keyCallback MacropadState::get_key_callback(uint8_t key)
{
	return m_keyPressedCallbacks[key];
}

keyCallbackGeneric MacropadState::get_key_generic_callback()
{
	return m_keyPressedGeneric;
}

int MacropadState::get_encoder_position()
{
	return m_encoderPosition;
}

int MacropadState::get_encoder_last_postition()
{
	return m_encoderLastPosition;
}

encoderCallback MacropadState::get_encoder_callback()
{
	return m_encoderCallback;
}

encoderPressedCallback MacropadState::get_encoder_pressed_callback()
{
	return m_encoderPressedCallback;
}

bool MacropadState::get_update_on_encoder_move()
{
	return m_updateOnEncoderMove;
}

bool MacropadState::get_update_on_encoder_press()
{
	return m_updateOnEncoderPress;
}

bool MacropadState::get_update_on_key_press()
{
	return m_updateOnKeyPress;
}
