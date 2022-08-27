#pragma once

#include "pico/stdlib.h"

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
    bool _defaultHigh;
    uint8_t _keyCount;
	uint8_t _firstGPIO;
    uint32_t _values;
    uint32_t _oldValues;
};
