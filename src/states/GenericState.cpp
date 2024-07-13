#include "states/PowerOffState.h"
#include "states/GenericState.h"
#include "states/common.h"
#include "MacropadState.h"
#include "Macropad.h"


GenericState::GenericState(MacropadState* parent)
    : MacropadState(parent), powerOffState(this) { };


void GenericState::KeyAny(uint8_t key, bool rising, bool falling)
{
    ripple(key, rising, falling);
}

void GenericState::Key1(bool rising, bool falling)
{
    return_to_parent_state(rising, falling);
}

void GenericState::Key2(bool rising, bool falling)
{
    press_keys<HID_KEY_GUI_LEFT, HID_KEY_ESCAPE>(rising, falling);
}

void GenericState::Key3(bool rising, bool falling)
{
    press_keys<HID_KEY_GUI_LEFT, HID_KEY_L>(rising, falling);
}

void GenericState::Key4(bool rising, bool falling)
{
    press_keys<HID_KEY_CONTROL_LEFT, HID_KEY_C>(rising, falling);
}

void GenericState::Key5(bool rising, bool falling)
{
    press_keys<HID_KEY_CONTROL_LEFT, HID_KEY_V>(rising, falling);
}

void GenericState::Key6(bool rising, bool falling)
{
    press_keys<HID_KEY_CONTROL_LEFT, HID_KEY_A>(rising, falling);
}

void GenericState::Key7(bool rising, bool falling)
{
    press_keys<HID_KEY_GUI_LEFT, HID_KEY_D>(rising, falling);
}

void GenericState::Key8(bool rising, bool falling)
{
    press_keys<HID_KEY_CONTROL_LEFT, HID_KEY_SHIFT_LEFT, HID_KEY_ESCAPE>(rising, falling);
}

void GenericState::Key9(bool rising, bool falling)
{
    press_keys<HID_KEY_GUI_LEFT, HID_KEY_SHIFT_LEFT, HID_KEY_S>(rising, falling);
}

void GenericState::Key12(bool rising, bool falling)
{
	(void) rising;

	if (falling) {
        powerOffState.Activate();
    }
}


void GenericState::EncoderHandler()
{
    encoder_volume(m_EncoderLastPosition, m_EncoderPosition);
}

void GenericState::EncoderPress(bool rising, bool falling)
{
    toggle_mute(rising, falling);
}


void GenericState::OledDraw(SH1106_SPI oled)
{
	const char* const labels[8][3] = {
		{"Back", "Lock", "Lock"},
		{"", "", "Win"},
		{"Copy", "Paste", "Select"},
		{"", "", ""},
		{"Desktop", "Task", "Screen"},
		{"", "Manager", "Shot"},
		{"", "", "Power"},
		{"", "", ""}
	};

	for (uint8_t y = 0; y < 8; y++) {
		for (uint8_t x = 0; x < 3; x++) {
			oled.gotoXY(x * 43, y);
			oled.print(labels[y][x]);
		}
	}
}

