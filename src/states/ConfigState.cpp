#include "states/ConfigState.h"
#include "states/common.h"
#include "MacropadState.h"
#include "RippleAnim.h"
#include "Macropad.h"
#include "Config.h"


ConfigState::ConfigState(MacropadState* parent)
    : MacropadState(parent)
{
    m_ActiveValue = ConfigValue::None;
    set_oled_automatic_updates(true, false, true);
}


void ConfigState::Activate()
{
    m_ActiveValue = ConfigValue::None;
    macropad::LoadState(this);
}


void ConfigState::KeyAny(uint8_t key, bool rising, bool falling)
{
    ripple(key, rising, falling);
}

void ConfigState::Key1(bool rising, bool falling)
{
    if (rising) {
        if (m_ActiveValue == ConfigValue::Slot) {
            Config::switchSlot(get_encoder_position() - 1);
        }
    }

    if (falling) {
        macropad::LoadParentState();
    }
}

void ConfigState::Key2(bool rising, bool falling)
{
    (void) falling;

    if (rising) {
        if (m_ActiveValue != ConfigValue::None) {
            SwitchActive<ConfigValue::None>(true, false);
        } else {
            Config::flush();
        }
    }
}

void ConfigState::Key3(bool rising, bool falling)
{
    SwitchActive<ConfigValue::Slot>(rising, falling);
}

void ConfigState::Key4(bool rising, bool falling)
{
    SwitchActive<ConfigValue::ColorOutR>(rising, falling);
}

void ConfigState::Key5(bool rising, bool falling)
{
    SwitchActive<ConfigValue::ColorOutG>(rising, falling);
}

void ConfigState::Key6(bool rising, bool falling)
{
    SwitchActive<ConfigValue::ColorOutB>(rising, falling);
}

void ConfigState::Key7(bool rising, bool falling)
{
    SwitchActive<ConfigValue::ColorInR>(rising, falling);
}

void ConfigState::Key8(bool rising, bool falling)
{
    SwitchActive<ConfigValue::ColorInG>(rising, falling);
}

void ConfigState::Key9(bool rising, bool falling)
{
    SwitchActive<ConfigValue::ColorInB>(rising, falling);
}

void ConfigState::Key10(bool rising, bool falling)
{
    SwitchActive<ConfigValue::Brightness>(rising, falling);
}


void ConfigState::EncoderHandler()
{
    if (m_EncoderPosition == m_EncoderLastPosition) {
        return;
    }

    CFG* cfg = Config::get();

    if (m_EncoderPosition < 0) {
        macropad::Encoder.setPosition(0);
        macropad::state->set_encoder_position(0);
    } else if (m_EncoderPosition > 255) {
        macropad::Encoder.setPosition(255);
        macropad::state->set_encoder_position(255);
    }

    switch (m_ActiveValue) {
        case ConfigValue::Slot:
            if (m_EncoderPosition < 1) {
                macropad::Encoder.setPosition(1);
                macropad::state->set_encoder_position(1);
            } else if (m_EncoderPosition > NUM_SLOTS) {
                macropad::Encoder.setPosition(NUM_SLOTS);
                macropad::state->set_encoder_position(NUM_SLOTS);
            }
            break;
        case ConfigValue::Brightness:
            cfg->brightness = get_encoder_position();
            ripple_anim_set_brightness(cfg->brightness);
            // m_PixelBrightness = cfg->brightness;
            break;
        case ConfigValue::ColorOutR:
            cfg->colorOutR = get_encoder_position();
            break;
        case ConfigValue::ColorOutG:
            cfg->colorOutG = get_encoder_position();
            break;
        case ConfigValue::ColorOutB:
            cfg->colorOutB = get_encoder_position();
            break;
        case ConfigValue::ColorInR:
            cfg->colorInR = get_encoder_position();
            break;
        case ConfigValue::ColorInG:
            cfg->colorInG = get_encoder_position();
            break;
        case ConfigValue::ColorInB:
            cfg->colorInB = get_encoder_position();
            break;
        case ConfigValue::None:
            break;
    }
}


void ConfigState::OledDraw(SH1106_SPI oled)
{
    if (m_ActiveValue == ConfigValue::None) {
        PrintKeys(oled);
        return;
    }

    oled.gotoXY(0, 0);
    oled.print("Active config slot: ");
    oled.print(Config::getSlot() + 1);

    oled.gotoXY(0, 1);
    switch (m_ActiveValue) {
        case ConfigValue::Slot:
            oled.print("Config slot: ");
            break;
        case ConfigValue::Brightness:
            oled.print("Brightness: ");
            break;
        case ConfigValue::ColorOutR:
            oled.print("Out Red: ");
            break;
        case ConfigValue::ColorOutG:
            oled.print("Out Green: ");
            break;
        case ConfigValue::ColorOutB:
            oled.print("Out Blue: ");
            break;
        case ConfigValue::ColorInR:
            oled.print("In Red: ");
            break;
        case ConfigValue::ColorInG:
            oled.print("In Green: ");
            break;
        case ConfigValue::ColorInB:
            oled.print("In Blue: ");
            break;
        default:
            oled.print("Unknown cfg option: ");
            break;
    }
    oled.print(get_encoder_position());
}

void ConfigState::PrintKeys(SH1106_SPI oled)
{
    const char* const labels[4][3] = {
        {"Back", "Save", "Slot"},
        {"Out R", "Out G", "Out B"},
        {"In R", "In G", "In B"},
        {"Bright", "", ""},
    };

    for (uint8_t y = 0; y < 4; y++) {
        for (uint8_t x = 0; x < 3; x++) {
            oled.gotoXY(x * 43, y * 2);
            oled.print(labels[y][x]);
        }
    }
}


template <ConfigValue val>
void ConfigState::SwitchActive(bool rising, bool falling)
{
    (void) falling;

    if (rising) {
        if (m_ActiveValue == ConfigValue::Slot) {
            Config::switchSlot(get_encoder_position() - 1);
        }

        if (m_ActiveValue != ConfigValue::None) {
            m_ActiveValue = ConfigValue::None;
        } else {
            m_ActiveValue = val;
        }

        CFG* cfg = Config::get();

        switch (m_ActiveValue) {
            case ConfigValue::Slot:
                macropad::Encoder.setPosition(Config::getSlot() + 1);
                macropad::state->set_encoder_position(Config::getSlot() + 1);
                break;
            case ConfigValue::Brightness:
                macropad::Encoder.setPosition(cfg->brightness);
                macropad::state->set_encoder_position(cfg->brightness);
                break;
            case ConfigValue::ColorOutR:
                macropad::Encoder.setPosition(cfg->colorOutR);
                macropad::state->set_encoder_position(cfg->colorOutR);
                break;
            case ConfigValue::ColorOutG:
                macropad::Encoder.setPosition(cfg->colorOutG);
                macropad::state->set_encoder_position(cfg->colorOutG);
                break;
            case ConfigValue::ColorOutB:
                macropad::Encoder.setPosition(cfg->colorOutB);
                macropad::state->set_encoder_position(cfg->colorOutB);
                break;
            case ConfigValue::ColorInR:
                macropad::Encoder.setPosition(cfg->colorInR);
                macropad::state->set_encoder_position(cfg->colorInR);
                break;
            case ConfigValue::ColorInG:
                macropad::Encoder.setPosition(cfg->colorInG);
                macropad::state->set_encoder_position(cfg->colorInG);
                break;
            case ConfigValue::ColorInB:
                macropad::Encoder.setPosition(cfg->colorInB);
                macropad::state->set_encoder_position(cfg->colorInB);
                break;
            case ConfigValue::None:
                macropad::Encoder.setPosition(0);
                macropad::state->set_encoder_position(0);
                break;
        }
    }
}

