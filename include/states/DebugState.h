#pragma once

#include "MacropadState.h"


class DebugState: public MacropadState
{
public:
    DebugState() = delete;
    DebugState(MacropadState* parent);

    void KeyAny(uint8_t key, bool rising, bool falling) override;

    void Key1(bool rising, bool falling) override;
    void Key2(bool rising, bool falling) override;
    void Key3(bool rising, bool falling) override;

    void OledDraw(SH1106_SPI oled) override;
};

