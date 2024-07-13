#pragma once

#include <stdint.h>


#define NUM_SLOTS 10


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
uint8_t getSlot();
CFG* savedValues();
void switchSlot(uint8_t slot);
void erase();
void eraseAll();
void flush();

}

