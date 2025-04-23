#pragma once

#include <stdint.h>
#include "MacropadState.h"


enum class KeyType {
    NORMAL,
    CONSUMER,
    SYSTEM,
};

class AnyKeyState: public MacropadState
{
public:
    AnyKeyState() = delete;
    AnyKeyState(MacropadState* parent);

    void KeyAny(uint8_t key, bool rising, bool falling) override;

    void EncoderHandler() override;
    void EncoderPress(bool rising, bool falling) override;
    void OledDraw(SH1106_SPI oled) override;

private:
    void CycleKeyType();
    void CycleEncoderSpeed();

private:
    uint16_t m_EncoderSpeed;
    uint16_t m_KeyCode;
    KeyType m_KeyType;
    bool m_KeyLocked;
};

class DebugState: public MacropadState
{
public:
    DebugState() = delete;
    DebugState(MacropadState* parent);

    void KeyAny(uint8_t key, bool rising, bool falling) override;

    void Key1(bool rising, bool falling) override;
    void Key2(bool rising, bool falling) override;
    void Key3(bool rising, bool falling) override;
    void Key4(bool rising, bool falling) override;
    void Key5(bool rising, bool falling) override;
    void Key6(bool rising, bool falling) override;
    void Key7(bool rising, bool falling) override;

    void OledDraw(SH1106_SPI oled) override;

private:
    void PrintKeys(SH1106_SPI oled);

private:
    AnyKeyState anyKeyState;
    bool m_ShowMem;
};

