#include <pico/stdlib.h>
#include "Keys.h"

Keys::Keys(uint8_t keyCount, bool defaultHigh /* = false */, uint8_t firstGPIO /* = 1 */)
	: m_DefaultHigh(defaultHigh), m_KeyCount(keyCount), m_FirstGPIO(firstGPIO) { }

void Keys::update(void) {
    m_OldValues = m_Values;
    m_Values = 0;
    for (uint8_t i = 0; i < m_KeyCount; i++) {
        bool gpioState = gpio_get(i + m_FirstGPIO);
        m_Values |= (m_DefaultHigh ? !gpioState : gpioState) << i;
    }
}

bool Keys::haveChanged(void) {
    return m_Values != m_OldValues;
}

bool Keys::getKeyValue(uint8_t keyNum) {
    return m_Values & (1 << keyNum);
}

bool Keys::getKeyLastValue(uint8_t keyNum) {
    return m_OldValues & (1 << keyNum);
}

bool Keys::getKeyRisingEdge(uint8_t keyNum) {
    return m_Values & (1 << keyNum) && !(m_OldValues & (1 << keyNum));
}

bool Keys::getKeyFallingEdge(uint8_t keyNum) {
    return !(m_Values & (1 << keyNum)) && m_OldValues & (1 << keyNum);
}
