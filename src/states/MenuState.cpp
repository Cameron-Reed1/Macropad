#include "states/GenericState.h"
#include "states/ConfigState.h"
#include "states/DebugState.h"
#include "states/TeamsState.h"
#include "states/TimerState.h"
#include "states/LinkState.h"

#include "states/MenuState.h"
#include "states/common.h"
#include "MacropadState.h"
#include "Macropad.h"


namespace MenuState {

void load_teams_state(bool rising, bool falling);
void load_generic_state(bool rising, bool falling);
void load_config_state(bool rising, bool falling);
void load_link_state(bool rising, bool falling);
void load_dev_state(bool rising, bool falling);
void load_timer_state(bool rising, bool falling);

void oled_draw(SH1106_SPI oled);

static MacropadState menuState(ripple,
        load_teams_state, load_generic_state, load_config_state,
        load_link_state, load_dev_state, load_timer_state,
        nullptr, nullptr, nullptr,
        nullptr, nullptr, /*load_dummy_state*/ nullptr,
        encoder_volume, toggle_mute, oled_draw);

void load_state()
{
    menuState.set_oled_automatic_updates(true);
    Macropad::get_instance().set_macropad_state(&menuState);
}

void load_teams_state(bool rising, bool falling)
{
	(void) rising;

	if (falling) {
        TeamsState::load_state(Macropad::get_instance().get_macropad_state());
    }
}

void load_generic_state(bool rising, bool falling)
{
	(void) rising;

	if (falling) {
        GenericState::load_state(Macropad::get_instance().get_macropad_state());
    }
}

void load_config_state(bool rising, bool falling)
{
	(void) rising;

	if (falling) {
        ConfigState::load_state(Macropad::get_instance().get_macropad_state());
	}
}

void load_link_state(bool rising, bool falling)
{
	(void) rising;

	if (falling) {
        LinkState::load_state(Macropad::get_instance().get_macropad_state());
	}
}

void load_dev_state(bool rising, bool falling)
{
	(void) rising;

	if (falling) {
        DebugState::load_state(Macropad::get_instance().get_macropad_state());
    }
}

void load_timer_state(bool rising, bool falling)
{
	(void) rising;

	if (falling) {
        TimerState::load_state(Macropad::get_instance().get_macropad_state());
    }
}


void oled_draw(SH1106_SPI oled)
{
    Macropad& macropad = Macropad::get_instance();

	oled.gotoXY(0, 0);
	oled.print("Encoder Position: ");
	oled.print(macropad.get_encoder_position());

    /* CFG* cfg = Config::get();
    oled.gotoXY(0, 1);
    oled.print("Config value: ");
    oled.print(cfg->brightness);

    CFG* cfg_saved = Config::savedValues();
    oled.gotoXY(0, 2);
    oled.print("Saved value: ");
    oled.print(cfg_saved->brightness); */

	for (int i = NUM_KEYS; i < NUM_KEYS; i++) {
		if (macropad.get_key_state(i + 1)) {
			oled.gotoXY((i % 3) * 48, (i / 3) + 4);
			oled.print("KEY");
			oled.print(i + 1);
		}
	}
}

}

