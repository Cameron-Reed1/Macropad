#pragma once

#include "MacropadState.h"
#include "PowerOffState.h"
#include <stdint.h>


class GenericState: public MacropadState
{
public:
    GenericState() = delete;
    GenericState(MacropadState* parent);

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
    void Key12(bool rising, bool falling) override;

    void EncoderHandler() override;
    void EncoderPress(bool rising, bool falling) override;
    void OledDraw(SH1106_SPI oled) override;

private:
    PowerOffState powerOffState;
};

