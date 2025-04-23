#include <hardware/gpio.h>
#include "Keys.h"


Key::Key(uint8_t pin, bool active_low /* = true */)
    : m_ActiveLow(active_low), m_Pin(pin), m_State(0) { }

void Key::Update()
{
    m_State = m_State << 1;
    bool new_state = gpio_get(m_Pin);
    if (m_ActiveLow) {
        new_state = !new_state;
    }
    m_State |= new_state;
}

bool Key::Pressed()
{
    return (m_State & 1) != 0;
}

bool Key::RisingEdge()
{
    return (m_State & 0b11) == 0b01;
}

bool Key::FallingEdge()
{
    return (m_State & 0b11) == 0b10;
}
