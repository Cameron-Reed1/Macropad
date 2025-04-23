#include <cstdint>
#include <pico/bootrom.h>
#include <malloc.h>
#include <stdint.h>

#include "states/DebugState.h"
#include "states/common.h"
#include "MacropadState.h"
#include "PinDefs.h"
#include "Config.h"


const char nibble_to_hex_map[16] = {
    '0', '1', '2', '3',
    '4', '5', '6', '7',
    '8', '9', 'A', 'B',
    'C', 'D', 'E', 'F',
};

uint32_t getTotalHeap() {
   extern char __StackLimit, __bss_end__;

   return &__StackLimit  - &__bss_end__;
}

uint32_t getFreeHeap() {
   struct mallinfo m = mallinfo();

   return getTotalHeap() - m.uordblks;
}


DebugState::DebugState(MacropadState* parent)
    : MacropadState(parent), anyKeyState(this), m_ShowMem(false) { }


void DebugState::KeyAny(uint8_t key, bool rising, bool falling)
{
    ripple(key, rising, falling);
}

void DebugState::Key1(bool rising, bool falling)
{
    return_to_parent_state(rising, falling);
}

void DebugState::Key2(bool rising, bool falling)
{
    (void) rising;
    (void) falling;

    reset_usb_boot(1 << LED, 0);
}

void DebugState::Key3(bool rising, bool falling)
{
    (void) falling;

    if (rising) {
        Config::eraseAll();
    }
}

void DebugState::Key4(bool rising, bool falling)
{
    (void) rising;

    if (falling) {
        anyKeyState.Activate();
    }
}

void DebugState::Key5(bool rising, bool falling)
{
    (void) falling;

    if (rising) {
        m_ShowMem = !m_ShowMem;
        macropad::UpdateOled();
    }
}

void DebugState::Key6(bool rising, bool falling)
{
    (void) rising;

    if (falling) {
        macropad::SendVendorCmd(0x0102, 0x0304);
    }
}

void DebugState::OledDraw(SH1106_SPI oled)
{
    if (m_ShowMem) {
        oled.gotoXY(0, 0);
        oled.print("Memory: ");
        oled.print(getFreeHeap());
        oled.print('/');
        oled.print(getTotalHeap());
    } else {
        PrintKeys(oled);
    }
}

void DebugState::PrintKeys(SH1106_SPI oled)
{
    const char* const labels[4][3] = {
        {"Back", "Boot", "Erase"},
        {"Any Key", "Memory", "Cmd"},
        {"", "", ""},
        {"", "", ""}
    };

    for (uint8_t y = 0; y < 4; y++) {
        for (uint8_t x = 0; x < 3; x++) {
            oled.gotoXY(x * 45, y * 2);
            oled.print(labels[y][x]);
        }
    }
}



AnyKeyState::AnyKeyState(MacropadState* parent)
    : MacropadState(parent), m_KeyCode(0), m_KeyLocked(false), m_EncoderSpeed(1), m_KeyType(KeyType::NORMAL)
{
    set_oled_automatic_updates(true, true, true);
};


void AnyKeyState::KeyAny(uint8_t key, bool rising, bool falling)
{
    ripple(key, rising, falling);

    if (key == 0) {
        return_to_parent_state(rising, falling);
        return;
    } else if (key == 1) {
        if (rising) {
            CycleKeyType();
        }
        return;
    } else if (key == 2) {
        if (rising) {
            CycleEncoderSpeed();
        }
        return;
    }

    if (rising) {
        switch (m_KeyType) {
            case KeyType::NORMAL:
                macropad::PressKey(m_KeyCode);
                break;
            case KeyType::CONSUMER:
                macropad::PressConsumerKey(m_KeyCode);
                break;
            case KeyType::SYSTEM:
                macropad::PressSystemKey(m_KeyCode);
                break;
        }
    }

    if (falling) {
        switch (m_KeyType) {
            case KeyType::NORMAL:
                macropad::ReleaseKey(m_KeyCode);
                break;
            case KeyType::CONSUMER:
                macropad::ReleaseConsumerKeys();
                break;
            case KeyType::SYSTEM:
                macropad::ReleaseSystemKeys();
                break;
        }
    }
}


void AnyKeyState::EncoderHandler()
{
    if (m_KeyLocked || m_EncoderPosition == m_EncoderLastPosition) {
        return;
    }

    if (m_EncoderPosition - m_EncoderLastPosition < 0) {
        // Position decreased
        if (m_KeyCode >= m_EncoderSpeed) {
            m_KeyCode -= m_EncoderSpeed;
        } else {
            m_KeyCode = 0;
        }
    } else if (m_EncoderPosition - m_EncoderLastPosition > 0) {
        // Position increased
        uint16_t max = m_KeyType == KeyType::NORMAL ? 0xFF : 0xFFFF;
        if (m_KeyCode <= max - m_EncoderSpeed) {
            m_KeyCode += m_EncoderSpeed;
        } else {
            m_KeyCode = max;
        }
    }
}

void AnyKeyState::EncoderPress(bool rising, bool falling)
{
    (void) falling;

    if (rising) {
        CycleEncoderSpeed();
    }
}


void AnyKeyState::OledDraw(SH1106_SPI oled)
{
    oled.gotoXY(0, 0);
    oled.print("Key type: ");
    switch (m_KeyType) {
        case KeyType::NORMAL:
            oled.print("Normal");
            break;
        case KeyType::CONSUMER:
            oled.print("Consumer");
            break;
        case KeyType::SYSTEM:
            oled.print("System");
            break;
    }

    oled.gotoXY(0, 1);
    oled.print("Key: 0x");
    if (m_KeyType != KeyType::NORMAL) {
        oled.print(nibble_to_hex_map[(m_KeyCode & 0xF000) >> 12]);
        oled.print(nibble_to_hex_map[(m_KeyCode & 0x0F00) >> 8]);
    }
    oled.print(nibble_to_hex_map[(m_KeyCode & 0x00F0) >> 4]);
    oled.print(nibble_to_hex_map[m_KeyCode & 0x000F]);

    oled.gotoXY(0, 2);
    oled.print("Speed: 0x");
    oled.print(m_EncoderSpeed, HEX);
}


void AnyKeyState::CycleKeyType()
{
    m_KeyCode = 0;
    switch (m_KeyType) {
        case KeyType::NORMAL:
            m_KeyType = KeyType::CONSUMER;
            break;
        case KeyType::CONSUMER:
            m_KeyType = KeyType::SYSTEM;
            break;
        case KeyType::SYSTEM:
            m_KeyType = KeyType::NORMAL;
            break;
    }
}

void AnyKeyState::CycleEncoderSpeed()
{
    switch (m_EncoderSpeed) {
        case 0x0001:
            m_EncoderSpeed = 0x0010;
            break;
        case 0x0010:
            m_EncoderSpeed = 0x0100;
            break;
        case 0x0100:
            m_EncoderSpeed = 0x1000;
            break;
        case 0x1000:
        default:
            m_EncoderSpeed = 0x0001;
            break;
    }
}

