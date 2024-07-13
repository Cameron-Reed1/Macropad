#pragma once

#include "MacropadState.h"


enum class ConfigValue {
    None,
    Slot,
    Brightness,
    ColorOutR,
    ColorOutG,
    ColorOutB,
    ColorInR,
    ColorInG,
    ColorInB,
};


class ConfigState: public MacropadState
{
public:
    ConfigState() = delete;
    ConfigState(MacropadState* parent);

    void Activate() override;

    void KeyAny(uint8_t key, bool rising, bool falling) override;

    void Key1(bool rising, bool falling) override;
    void Key2(bool rising, bool falling) override;
    void Key3(bool rising, bool falling) override;
    void Key4(bool rising, bool falling) override;
    void Key5(bool rising, bool falling) override;
    void Key6(bool rising, bool falling) override;
    void Key7(bool rising, bool falling) override;
    void Key8(bool rising, bool falling) override;
    void Key9(bool rising, bool falling) override;
    void Key10(bool rising, bool falling) override;

    void EncoderHandler() override;
    void OledDraw(SH1106_SPI oled) override;

private:
    void PrintKeys(SH1106_SPI oled);

    template <ConfigValue val>
    void SwitchActive(bool rising, bool falling);

private:
    ConfigValue m_ActiveValue;
};

