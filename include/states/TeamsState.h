#pragma once

#include "MacropadState.h"


class TeamsState: public MacropadState
{
public:
    TeamsState() = delete;
    TeamsState(MacropadState* parent);

    void KeyAny(uint8_t key, bool rising, bool falling) override;

    void Key1(bool rising, bool falling) override;
    void Key7(bool rising, bool falling) override;
    void Key8(bool rising, bool falling) override;
    void Key9(bool rising, bool falling) override;
    void Key10(bool rising, bool falling) override;
    void Key11(bool rising, bool falling) override;
    void Key12(bool rising, bool falling) override;

    void EncoderHandler() override;
    void EncoderPress(bool rising, bool falling) override;
    
    void OledDraw(SH1106_SPI oled) override;
};

