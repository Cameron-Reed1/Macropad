#pragma once

#include <stdint.h>

#define KEY_COUNT 12
#define PIXEL_COUNT 12
#define FRAME_COUNT 6
#define FRAME_TIME_MS 100
#define MAX_CONCURRENT_ANIMATIONS 24
#define FRAME_TIME_OVERLAP_MS 25

enum RippleAnimDir : int8_t {
	DIR_IN = -1,
	DIR_OUT = 1
};

void ripple_anim_setup();
void ripple_anim_set_brightness(uint8_t brightness);
void ripple_anim_run(uint32_t start_key, uint8_t color_red, uint8_t color_green, uint8_t color_blue, RippleAnimDir direction);
