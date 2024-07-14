#pragma once

#include <stdint.h>

class Keys {
public:
    Keys(uint8_t keyCount, bool defaultHigh = false, uint8_t firstGPIO = 1);
    void update(void);
    bool haveChanged(void);
    bool getKeyValue(uint8_t keyNum);
    bool getKeyLastValue(uint8_t keyNum);
    bool getKeyRisingEdge(uint8_t keyNum);
    bool getKeyFallingEdge(uint8_t keyNum);

private:
    bool m_DefaultHigh;
    uint8_t m_FirstGPIO;
    uint8_t m_KeyCount;
    uint32_t m_Values;
    uint32_t m_OldValues;
};
