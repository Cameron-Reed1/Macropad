#include <pico/bootrom.h>

#include "states/MenuState.h"
#include "RippleAnim.h"
#include "Macropad.h"
#include "Config.h"


int main(void) {
    Macropad& macropad = Macropad::get_instance();
    CFG* cfg = Config::read();

	macropad.init(true, true, true, false, false, true, false);
    MenuState::load_state();
	ripple_anim_setup();
    ripple_anim_set_brightness(cfg->brightness);

	while (true) {
		macropad.run();
	}

	return 0;
}

