#include <pico/types.h>
#include <pico/time.h>
#include <stdint.h>

#include "states/common.h"
#include "RippleAnim.h"
#include "Macropad.h"
#include "Config.h"


static absolute_time_t press_start[KEY_COUNT] = {0};


void ripple(uint8_t key, bool rising, bool falling)
{
    CFG* cfg = Config::get();

	if (rising) {
		press_start[key] = get_absolute_time();
		ripple_anim_run(key, cfg->colorOutR, cfg->colorOutG, cfg->colorOutB, DIR_OUT);
	} else if (falling) {
		if (get_absolute_time() - press_start[key] > 320 * 1000)
			ripple_anim_run(key, cfg->colorInR, cfg->colorInG, cfg->colorInB, DIR_IN);
	}
}

void return_to_parent_state(bool rising, bool falling)
{
	(void) rising;

	if (falling)
        Macropad::get_instance().load_parent_state();
}

