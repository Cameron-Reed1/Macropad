#pragma once

#include <stdint.h>

struct CFG {
    uint8_t magic = 0x64;

    uint8_t brightness;

    uint8_t colorOutR;
    uint8_t colorOutG;
    uint8_t colorOutB;

    uint8_t colorInR;
    uint8_t colorInG;
    uint8_t colorInB;
};

namespace Config {
CFG* read();
CFG* get();
void erase();
CFG* savedValues();
void flush();
}

