#include "states/MenuState.h"
#include "RippleAnim.h"
#include "Macropad.h"
#include "Config.h"


int main(void)
{
    MenuState menuState;
    CFG* cfg = Config::read();

    macropad::init(true, true, true, false, true);
    menuState.Activate();
    ripple_anim_setup();
    ripple_anim_set_brightness(cfg->brightness);

    while (true) {
        macropad::tick();
    }

    return 0;
}

