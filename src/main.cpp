#include "states/MenuState.h"
#include "RippleAnim.h"
#include "Macropad.h"
#include "Config.h"


int main(void)
{
    Macropad& macropad = Macropad::get_instance();
    MenuState menuState = MenuState();
    CFG* cfg = Config::read();

	macropad.init(true, true, true, false, false, true, false);
    menuState.Activate();
	ripple_anim_setup();
    ripple_anim_set_brightness(cfg->brightness);

	while (true) {
		macropad.run();
	}

	return 0;
}

