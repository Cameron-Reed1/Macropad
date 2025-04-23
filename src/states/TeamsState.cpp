#include "states/TeamsState.h"
#include "states/common.h"
#include "MacropadState.h"
#include "Macropad.h"


TeamsState::TeamsState(MacropadState* parent)
    : MacropadState(parent) { }


void TeamsState::KeyAny(uint8_t key, bool rising, bool falling)
{
    ripple(key, rising, falling);
}

void TeamsState::Key1(bool rising, bool falling)
{
    return_to_parent_state(rising, falling);
}

void TeamsState::Key7(bool rising, bool falling)
{
    (void) falling;

    static Macro* macro = nullptr;
    if (macro == nullptr) {
        macro = new Macro(6);
        macro->addKeyPressStep(HID_KEY_CONTROL_LEFT, 1, HID_KEY_SHIFT_LEFT, HID_KEY_P).addSleepStep(500).addKeyPressStep(HID_KEY_ENTER).addKeyPressStep(HID_KEY_TAB, 2).addKeyPressStep(HID_KEY_ENTER);
    }

    if (rising) {
        macropad::PlayMacro(macro);
    }
}

void TeamsState::Key8(bool rising, bool falling)
{
    press_keys<HID_KEY_CONTROL_LEFT, HID_KEY_SHIFT_LEFT, HID_KEY_H>(rising, falling);
}

void TeamsState::Key9(bool rising, bool falling)
{
    press_keys<HID_KEY_CONTROL_LEFT, HID_KEY_SHIFT_LEFT, HID_KEY_K>(rising, falling);
}

void TeamsState::Key10(bool rising, bool falling)
{
    press_keys<HID_KEY_CONTROL_LEFT, HID_KEY_SPACE>(rising, falling);
}

void TeamsState::Key11(bool rising, bool falling)
{
    press_keys<HID_KEY_CONTROL_LEFT, HID_KEY_SHIFT_LEFT, HID_KEY_M>(rising, falling);
}

void TeamsState::Key12(bool rising, bool falling)
{
    press_keys<HID_KEY_CONTROL_LEFT, HID_KEY_SHIFT_LEFT, HID_KEY_O>(rising, falling);
}


void TeamsState::EncoderHandler()
{
    encoder_volume(m_EncoderLastPosition, m_EncoderPosition);
}

void TeamsState::EncoderPress(bool rising, bool falling)
{
    toggle_mute(rising, falling);
}


void TeamsState::OledDraw(SH1106_SPI oled)
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

