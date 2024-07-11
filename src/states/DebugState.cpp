#include <pico/bootrom.h>

#include "states/DebugState.h"
#include "states/common.h"
#include "MacropadState.h"
#include "Macropad.h"
#include "Config.h"


namespace DebugState {

void enterBootloader(bool rising, bool falling);
void saveConfig(bool rising, bool falling);
void eraseConfig(bool rising, bool falling);

void oled_draw(SH1106_SPI oled);

static MacropadState debugState(ripple,
        return_to_parent_state, enterBootloader, saveConfig,
        eraseConfig, nullptr, nullptr,
        nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr,
        nullptr, nullptr, oled_draw);

void load_state(MacropadState* parent)
{
    debugState.set_parent_state(parent);
    debugState.set_oled_automatic_updates(false);
    Macropad::get_instance().set_macropad_state(&debugState);
}

void enterBootloader(bool rising, bool falling)
{
	(void) rising;
	(void) falling;

	reset_usb_boot(1 << LED, 0);
}

void saveConfig(bool rising, bool falling)
{
    (void) falling;

    if (rising) {
        Config::flush();
    }
}

void eraseConfig(bool rising, bool falling)
{
    (void) falling;

    if (rising) {
        Config::erase();
    }
}

void oled_draw(SH1106_SPI oled)
{
	const char* const functions[4][3] = {
		{"Back", "Boot", "Save"},
		{"Erase", "", ""},
		{"", "", ""},
		{"", "", ""}
	};

	for (uint8_t y = 0; y < 4; y++) {
		for (uint8_t x = 0; x < 3; x++) {
			oled.gotoXY(x * 45, y * 2);
			oled.print(functions[y][x]);
		}
	}
}

} // namespace DebugState

