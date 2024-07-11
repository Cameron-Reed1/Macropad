#include "RippleAnim.h"
#include <pico/util/queue.h>

static queue_t comm_q;

struct AnimationData {
	uint8_t start_key;
	uint8_t current_frame;
	RippleAnimDir direction;
	uint32_t color;
};

bool animations[PIXEL_COUNT][FRAME_COUNT][PIXEL_COUNT] {
	{
		{1, 0, 0,	0, 0, 0,	0, 0, 0,	0, 0, 0},
		{0, 1, 0,	1, 0, 0,	0, 0, 0,	0, 0, 0},
		{0, 0, 1,	0, 1, 0,	1, 0, 0,	0, 0, 0},
		{0, 0, 0,	0, 0, 1,	0, 1, 0,	1, 0, 0},
		{0, 0, 0,	0, 0, 0,	0, 0, 1,	0, 1, 0},
		{0, 0, 0,	0, 0, 0,	0, 0, 0,	0, 0, 1}
	},
	{
		{0, 1, 0,	0, 0, 0,	0, 0, 0,	0, 0, 0},
		{1, 0, 1,	0, 1, 0,	0, 0, 0,	0, 0, 0},
		{0, 0, 0,	1, 0, 1,	0, 1, 0,	0, 0, 0},
		{0, 0, 0,	0, 0, 0,	1, 0, 1,	0, 1, 0},
		{0, 0, 0,	0, 0, 0,	0, 0, 0,	1, 0, 1},
		{0, 0, 0,	0, 0, 0,	0, 0, 0,	0, 0, 0}
	},
	{
		{0, 0, 1,	0, 0, 0,	0, 0, 0,	0, 0, 0},
		{0, 1, 0,	0, 0, 1,	0, 0, 0,	0, 0, 0},
		{1, 0, 0,	0, 1, 0,	0, 0, 1,	0, 0, 0},
		{0, 0, 0,	1, 0, 0,	0, 1, 0,	0, 0, 1},
		{0, 0, 0,	0, 0, 0,	1, 0, 0,	0, 1, 0},
		{0, 0, 0,	0, 0, 0,	0, 0, 0,	1, 0, 0}
	},
	{
		{0, 0, 0,	1, 0, 0,	0, 0, 0,	0, 0, 0},
		{1, 0, 0,	0, 1, 0,	1, 0, 0,	0, 0, 0},
		{0, 1, 0,	0, 0, 1,	0, 1, 0,	1, 0, 0},
		{0, 0, 1,	0, 0, 0,	0, 0, 1,	0, 1, 0},
		{0, 0, 0,	0, 0, 0,	0, 0, 0,	0, 0, 1},
		{0, 0, 0,	0, 0, 0,	0, 0, 0,	0, 0, 0}
	},
	{
		{0, 0, 0,	0, 1, 0,	0, 0, 0,	0, 0, 0},
		{0, 1, 0,	1, 0, 1,	0, 1, 0,	0, 0, 0},
		{1, 0, 1,	0, 0, 0,	1, 0, 1,	0, 1, 0},
		{0, 0, 0,	0, 0, 0,	0, 0, 0,	1, 0, 1},
		{0, 0, 0,	0, 0, 0,	0, 0, 0,	0, 0, 0},
		{0, 0, 0,	0, 0, 0,	0, 0, 0,	0, 0, 0}
	},
	{
		{0, 0, 0,	0, 0, 1,	0, 0, 0,	0, 0, 0},
		{0, 0, 1,	0, 1, 0,	0, 0, 1,	0, 0, 0},
		{0, 1, 0,	1, 0, 0,	0, 1, 0,	0, 0, 1},
		{1, 0, 0,	0, 0, 0,	1, 0, 0,	0, 1, 0},
		{0, 0, 0,	0, 0, 0,	0, 0, 0,	1, 0, 0},
		{0, 0, 0,	0, 0, 0,	0, 0, 0,	0, 0, 0}
	},
	{
		{0, 0, 0,	0, 0, 0,	1, 0, 0,	0, 0, 0},
		{0, 0, 0,	1, 0, 0,	0, 1, 0,	1, 0, 0},
		{1, 0, 0,	0, 1, 0,	0, 0, 1,	0, 1, 0},
		{0, 1, 0,	0, 0, 1,	0, 0, 0,	0, 0, 1},
		{0, 0, 1,	0, 0, 0,	0, 0, 0,	0, 0, 0},
		{0, 0, 0,	0, 0, 0,	0, 0, 0,	0, 0, 0}
	},
	{
		{0, 0, 0,	0, 0, 0,	0, 1, 0,	0, 0, 0},
		{0, 0, 0,	0, 1, 0,	1, 0, 1,	0, 1, 0},
		{0, 1, 0,	1, 0, 1,	0, 0, 0,	1, 0, 1},
		{1, 0, 1,	0, 0, 0,	0, 0, 0,	0, 0, 0},
		{0, 0, 0,	0, 0, 0,	0, 0, 0,	0, 0, 0},
		{0, 0, 0,	0, 0, 0,	0, 0, 0,	0, 0, 0}
	},
	{
		{0, 0, 0,	0, 0, 0,	0, 0, 1,	0, 0, 0},
		{0, 0, 0,	0, 0, 1,	0, 1, 0,	0, 0, 1},
		{0, 0, 1,	0, 1, 0,	1, 0, 0,	0, 1, 0},
		{0, 1, 0,	1, 0, 0,	0, 0, 0,	1, 0, 0},
		{1, 0, 0,	0, 0, 0,	0, 0, 0,	0, 0, 0},
		{0, 0, 0,	0, 0, 0,	0, 0, 0,	0, 0, 0}
	},
	{
		{0, 0, 0,	0, 0, 0,	0, 0, 0,	1, 0, 0},
		{0, 0, 0,	0, 0, 0,	1, 0, 0,	0, 1, 0},
		{0, 0, 0,	1, 0, 0,	0, 1, 0,	0, 0, 1},
		{1, 0, 0,	0, 1, 0,	0, 0, 1,	0, 0, 0},
		{0, 1, 0,	0, 0, 1,	0, 0, 0,	0, 0, 0},
		{0, 0, 1,	0, 0, 0,	0, 0, 0,	0, 0, 0}
	},
	{
		{0, 0, 0,	0, 0, 0,	0, 0, 0,	0, 1, 0},
		{0, 0, 0,	0, 0, 0,	0, 1, 0,	1, 0, 1},
		{0, 0, 0,	0, 1, 0,	1, 0, 1,	0, 0, 0},
		{0, 1, 0,	1, 0, 1,	0, 0, 0,	0, 0, 0},
		{1, 0, 1,	0, 0, 0,	0, 0, 0,	0, 0, 0},
		{0, 0, 0,	0, 0, 0,	0, 0, 0,	0, 0, 0}
	},
	{
		{0, 0, 0,	0, 0, 0,	0, 0, 0,	0, 0, 1},
		{0, 0, 0,	0, 0, 0,	0, 0, 1,	0, 1, 0},
		{0, 0, 0,	0, 0, 1,	0, 1, 0,	1, 0, 0},
		{0, 0, 1,	0, 1, 0,	1, 0, 0,	0, 0, 0},
		{0, 1, 0,	1, 0, 0,	0, 0, 0,	0, 0, 0},
		{1, 0, 0,	0, 0, 0,	0, 0, 0,	0, 0, 0}
	}
};

void ripple_anim_setup() {
	queue_init(&comm_q, 4, 12);
	multicore_launch_core1(ripple_anim_second_core);
}

void ripple_anim_set_brightness(uint8_t brightness)
{
	uint32_t data = 'b' | (brightness << 8);
	queue_add_blocking(&comm_q, &data);
}

void ripple_anim_run(uint32_t start_key, uint8_t color_red, uint8_t color_green, uint8_t color_blue, RippleAnimDir direction) {
	bool added = queue_try_add(&comm_q, &start_key);

	if (added) {
		uint32_t data = (direction << 24) | (color_red << 16) | (color_green << 8) | (color_blue);
		queue_add_blocking(&comm_q, &data);
	}
}

void ripple_anim_second_core() {
	AnimationData* active_animations[MAX_CONCURRENT_ANIMATIONS] = { nullptr };
	uint8_t active_animation_count = 0;
	Adafruit_NeoPixel pixels(PIXEL_COUNT, NEOPIXEL, NEO_GRB + NEO_KHZ800);
	for (uint8_t pixel = 0; pixel < PIXEL_COUNT; pixel++) {
		pixels.setPixelColor(pixel, 0x00);
	}
	pixels.setBrightness(80);
	pixels.show();
	while (true) {
		uint32_t start_key;
		queue_remove_blocking(&comm_q, &start_key);

		if ((start_key & 0xff) == 'b') {
			uint32_t brightness = (start_key & 0xff00) >> 8;
			pixels.setBrightness(brightness);
			continue;
		}

		if (start_key >= KEY_COUNT)
			continue;

		uint32_t data;
		queue_remove_blocking(&comm_q, &data);
		uint32_t color = data & 0x00ffffff;
		RippleAnimDir direction = (RippleAnimDir) ((data & 0xff000000) >> 24);
		uint8_t red = (color & (0xff << 16)) >> 16;
		uint8_t green = (color & (0xff << 8)) >> 8;
		uint8_t blue = color & 0xff;

		color = pixels.Color(red, green, blue);

		active_animations[0] = new AnimationData;
		active_animations[0]->start_key = start_key;
		active_animations[0]->current_frame = (FRAME_COUNT - 1) * (direction == DIR_IN);
		active_animations[0]->direction = direction;
		active_animations[0]->color = color;
		active_animation_count = 1;

		while (active_animation_count) {
			for (uint8_t pixel = 0; pixel < PIXEL_COUNT; pixel++) {
				pixels.setPixelColor(pixel, 0x00);
			}

			for (uint8_t a = 0; a < MAX_CONCURRENT_ANIMATIONS; a++) {
				if (active_animations[a] == nullptr)
					continue;

				for (uint8_t i = 0; i < PIXEL_COUNT; i++) {
					if (animations[active_animations[a]->start_key][active_animations[a]->current_frame][i])
						pixels.setPixelColor(i, active_animations[a]->color);
				}
			}
			pixels.show();

			sleep_ms(FRAME_TIME_MS - FRAME_TIME_OVERLAP_MS);

			for (uint8_t a = 0; a < MAX_CONCURRENT_ANIMATIONS; a++) {
				if (active_animations[a] == nullptr)
					continue;

				for (uint8_t i = 0; i < PIXEL_COUNT; i++) {
					if (animations[active_animations[a]->start_key][active_animations[a]->current_frame][i]
						|| (animations[active_animations[a]->start_key][active_animations[a]->current_frame + (active_animations[a]->direction == DIR_IN ? -1 : 1)][i]
						&& active_animations[a]->current_frame != (FRAME_COUNT - 1) * (active_animations[a]->direction == DIR_OUT))
					) {
						pixels.setPixelColor(i, active_animations[a]->color);
					}
				}
			}
			pixels.show();

			sleep_ms(FRAME_TIME_OVERLAP_MS);

			for (uint8_t a = 0; a < MAX_CONCURRENT_ANIMATIONS; a++) {
				if (active_animations[a] == nullptr)
					continue;

				if (active_animations[a]->current_frame == (FRAME_COUNT - 1) * (active_animations[a]->direction == DIR_OUT)) {
					delete active_animations[a];
					active_animations[a] = nullptr;
				}
			}

			active_animation_count = 0;
			for (uint8_t a = 0; a < MAX_CONCURRENT_ANIMATIONS; a++) {
				if (active_animations[a] == nullptr)
					continue;

				active_animations[a]->current_frame += active_animations[a]->direction;

				if (a != active_animation_count) {
					active_animations[active_animation_count] = active_animations[a];
					active_animations[a] = nullptr;
				}

				active_animation_count++;
			}

			if (active_animation_count < MAX_CONCURRENT_ANIMATIONS) {
				while (queue_try_remove(&comm_q, &start_key)) {
					if ((start_key & 0xff) == 'b') {
						pixels.setBrightness((start_key & 0xff00) >> 8);
					} else if (start_key < KEY_COUNT) {
						uint32_t data;
						queue_remove_blocking(&comm_q, &data);
						uint32_t color = data & 0x00ffffff;
						RippleAnimDir direction = (RippleAnimDir) ((data & 0xff000000) >> 24);

						uint8_t red = (color & (0xff << 16)) >> 16;
						uint8_t green = (color & (0xff << 8)) >> 8;
						uint8_t blue = color & 0xff;

						color = pixels.Color(red, green, blue);

						bool foundMatch = false;
						for (uint8_t a = 0; active_animations[a] != nullptr && a < MAX_CONCURRENT_ANIMATIONS; a++) {
							if (active_animations[a]->start_key == start_key && active_animations[a]->direction == DIR_OUT && direction == DIR_IN) {
								active_animations[a]->direction = direction;
								active_animations[a]->color = color;
								foundMatch = true;
								break;
							}
						}
						if (!foundMatch) {
							active_animations[active_animation_count] = new AnimationData;
							active_animations[active_animation_count]->start_key = start_key;
							active_animations[active_animation_count]->current_frame = (FRAME_COUNT - 1) * (direction == DIR_IN);
							active_animations[active_animation_count]->direction = direction;
							active_animations[active_animation_count]->color = color;

							active_animation_count++;
						}
					}
				}
			}
		}

		for (uint8_t i = 0; i < PIXEL_COUNT; i++) {
			pixels.setPixelColor(i, 0x00);
		}
		pixels.show();
	}
}

