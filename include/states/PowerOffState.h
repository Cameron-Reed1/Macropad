#pragma once

#include "MacropadState.h"


class PowerOffState: public MacropadState
{
public:
    PowerOffState() = delete;
    PowerOffState(MacropadState* parent);

    void KeyAny(uint8_t key, bool rising, bool falling) override;

    void Key12(bool rising, bool falling) override;

    void OledDraw(SH1106_SPI oled) override;
};

