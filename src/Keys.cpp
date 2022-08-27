#include "Keys.h"

Keys::Keys(uint8_t keyCount, bool defaultHigh /* = false */, uint8_t firstGPIO /* = 1 */)
	: _defaultHigh(defaultHigh), _keyCount(keyCount), _firstGPIO(firstGPIO) { }

void Keys::update(void) {
    _oldValues = _values;
    _values = 0;
    for (uint8_t i = 0; i < _keyCount; i++) {
        bool gpioState = gpio_get(i + _firstGPIO);
        _values |= (_defaultHigh ? !gpioState : gpioState) << i;
    }
}

bool Keys::haveChanged(void) {
    return _values != _oldValues;
}

bool Keys::getKeyValue(uint8_t keyNum) {
    return _values & (1 << (keyNum - 1));
}

bool Keys::getKeyLastValue(uint8_t keyNum) {
    return _oldValues & (1 << (keyNum - 1));
}

bool Keys::getKeyRisingEdge(uint8_t keyNum) {
    return _values & (1 << (keyNum - 1)) && !(_oldValues & (1 << (keyNum - 1)));
}

bool Keys::getKeyFallingEdge(uint8_t keyNum) {
    return !(_values & (1 << (keyNum - 1))) && _oldValues & (1 << (keyNum - 1));
}
