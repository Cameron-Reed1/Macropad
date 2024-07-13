#include "MacropadState.h"
#include "Macropad.h"


MacropadState::MacropadState(MacropadState* parent)
    : m_Parent(parent) { }


MacropadState* MacropadState::get_parent_state()
{
	return m_Parent;
}

void MacropadState::set_parent_state(MacropadState* parent)
{
	m_Parent = parent;
}

void MacropadState::set_oled_automatic_updates(bool enable /* = true */)
{
	this->set_oled_automatic_updates(enable, enable, enable);
}

void MacropadState::set_oled_automatic_updates(bool on_encoder_move, bool on_encoder_press, bool on_key_press)
{
	m_UpdateOnEncoderMove = on_encoder_move;
	m_UpdateOnEncoderPress = on_encoder_press;
	m_UpdateOnKeyPress = on_key_press;
}

void MacropadState::set_pixels_brightness(uint16_t brightness)
{
	m_PixelBrightness = brightness;
}

void MacropadState::set_pixel_color(uint8_t pixel, uint32_t color)
{
    m_PixelColors[pixel] = color;
}

void MacropadState::set_encoder_position(int newPos)
{
	m_EncoderLastPosition = m_EncoderPosition;
	m_EncoderPosition = newPos;
}

uint16_t MacropadState::get_pixels_brightness()
{
	return m_PixelBrightness;
}

uint32_t MacropadState::get_pixel_color(uint8_t pixel)
{
	return m_PixelColors[pixel];
}

int MacropadState::get_encoder_position()
{
	return m_EncoderPosition;
}

int MacropadState::get_encoder_last_postition()
{
	return m_EncoderLastPosition;
}

bool MacropadState::get_update_on_encoder_move()
{
	return m_UpdateOnEncoderMove;
}

bool MacropadState::get_update_on_encoder_press()
{
	return m_UpdateOnEncoderPress;
}

bool MacropadState::get_update_on_key_press()
{
	return m_UpdateOnKeyPress;
}

void MacropadState::Activate()
{
	Macropad::get_instance().set_macropad_state(this);
}


void MacropadState::Key(uint8_t key, bool rising, bool falling)
{
    switch (key) {
        case 0:
            Key1(rising, falling);
            break;

        case 1:
            Key2(rising, falling);
            break;

        case 2:
            Key3(rising, falling);
            break;

        case 3:
            Key4(rising, falling);
            break;

        case 4:
            Key5(rising, falling);
            break;

        case 5:
            Key6(rising, falling);
            break;

        case 6:
            Key7(rising, falling);
            break;

        case 7:
            Key8(rising, falling);
            break;

        case 8:
            Key9(rising, falling);
            break;

        case 9:
            Key10(rising, falling);
            break;

        case 10:
            Key11(rising, falling);
            break;

        case 11:
            Key12(rising, falling);
            break;

        default:
            return;
    }

    KeyAny(key, rising, falling);
}

