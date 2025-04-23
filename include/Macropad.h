#pragma once

#include <cstdint>
#include <pico/stdio.h>
#include <hardware/spi.h>
#include "RotaryEncoder.h"
#include "MacropadState.h"
#include "Adafruit_NeoPixel.hpp"
#include "SH1106_SPI.h"
#include "Macro.h"
#include "Keys.h"

#define SPI_PORT spi1
#define NUM_KEYS 12
#define NUM_PIXELS NUM_KEYS
#define MEGAHERTZ 1000 * 1000
#define VENDOR_CMD_QUEUE_LEN 8


namespace macropad {

extern MacropadState* state;
extern Key Keys[NUM_KEYS];
extern Key EncoderBtn;
extern RotaryEncoder Encoder;
extern Adafruit_NeoPixel Pixels;
extern SH1106_SPI Oled;
extern uint32_t VendorCmds[VENDOR_CMD_QUEUE_LEN];
extern uint8_t VendorCmdCount;
extern uint32_t ComputerID;

void init(bool tinyUSB, bool keys, bool encoder, bool pixels, bool oled);
void tick();

void LoadState(MacropadState* newState);
void LoadParentState();

bool PressKey(uint8_t key);
bool ReleaseKey(uint8_t key);
bool PressConsumerKey(uint16_t key);
bool ReleaseConsumerKeys();
bool PressSystemKey(uint16_t key);
bool ReleaseSystemKeys();

bool SendVendorCmd(uint16_t category, uint16_t command);
void ClearVendorCmds(uint8_t num);

bool PlayMacro(Macro* macro);
void Type(const char* str);

void UpdateOled();

}; // namespace macropad

