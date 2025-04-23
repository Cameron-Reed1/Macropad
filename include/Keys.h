#pragma once

#include <stdint.h>


class Key {
public:
    Key(uint8_t pin, bool active_low = true);

    void Update();
    bool Pressed();
    bool RisingEdge();
    bool FallingEdge();

private:
    bool m_ActiveLow;
    uint8_t m_Pin;
    uint8_t m_State;
};
