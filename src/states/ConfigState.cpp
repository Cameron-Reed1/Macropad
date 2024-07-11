#include "states/ConfigState.h"
#include "states/common.h"
#include "MacropadState.h"
#include "RippleAnim.h"
#include "Macropad.h"
#include "Config.h"


namespace ConfigState {

enum class ConfigValue {
    None,
    Brightness,
    ColorOutR,
    ColorOutG,
    ColorOutB,
    ColorInR,
    ColorInG,
    ColorInB,
};

template <ConfigValue val>
void switch_active(bool rising, bool falling);

void save(bool rising, bool falling);
constexpr keyCallback brightness = switch_active<ConfigValue::Brightness>;
constexpr keyCallback colorOutR = switch_active<ConfigValue::ColorOutR>;
constexpr keyCallback colorOutG = switch_active<ConfigValue::ColorOutG>;
constexpr keyCallback colorOutB = switch_active<ConfigValue::ColorOutB>;
constexpr keyCallback colorInR = switch_active<ConfigValue::ColorInR>;
constexpr keyCallback colorInG = switch_active<ConfigValue::ColorInG>;
constexpr keyCallback colorInB = switch_active<ConfigValue::ColorInB>;

void encoder_handler(int last_position, int new_position);
void oled_draw(SH1106_SPI oled);
void printKeys(SH1106_SPI oled);


static ConfigValue activeValue = ConfigValue::None;
static MacropadState configState(ripple,
        return_to_parent_state, save, brightness,
        colorOutR, colorOutG, colorOutB,
        colorInR, colorInG, colorInB,
        nullptr, nullptr, nullptr,
        encoder_handler, nullptr, oled_draw);


void load_state(MacropadState* parent)
{
    CFG* cfg = Config::get();

    activeValue = ConfigValue::None;
    configState.set_parent_state(parent);
    configState.set_encoder_position(cfg->brightness);
    configState.set_oled_automatic_updates(true, false, true);
	Macropad::get_instance().set_macropad_state(&configState);
}

void save(bool rising, bool falling)
{
    (void) rising;

    if (falling) {
        Config::flush();
    }
}

void encoder_handler(int last_position, int new_position)
{
    if (new_position == last_position) {
        return;
    }

    Macropad& macropad = Macropad::get_instance();
    CFG* cfg = Config::get();

	if (new_position < 0) {
	    macropad.set_encoder_position(0);
	} else if (new_position > 255) {
	    macropad.set_encoder_position(255);
	}
    
    switch (activeValue) {
        case ConfigValue::Brightness:
            cfg->brightness = macropad.get_encoder_position();
            ripple_anim_set_brightness(cfg->brightness);
            break;
        case ConfigValue::ColorOutR:
            cfg->colorOutR = macropad.get_encoder_position();
            break;
        case ConfigValue::ColorOutG:
            cfg->colorOutG = macropad.get_encoder_position();
            break;
        case ConfigValue::ColorOutB:
            cfg->colorOutB = macropad.get_encoder_position();
            break;
        case ConfigValue::ColorInR:
            cfg->colorInR = macropad.get_encoder_position();
            break;
        case ConfigValue::ColorInG:
            cfg->colorInG = macropad.get_encoder_position();
            break;
        case ConfigValue::ColorInB:
            cfg->colorInB = macropad.get_encoder_position();
            break;
        case ConfigValue::None:
            break;
    }
}

void oled_draw(SH1106_SPI oled)
{
    if (activeValue == ConfigValue::None) {
        printKeys(oled);
        return;
    }

    Macropad& macropad = Macropad::get_instance();

	oled.gotoXY(0, 0);
    switch (activeValue) {
        case ConfigValue::Brightness:
	        oled.print("Brightness: ");
            break;
        case ConfigValue::ColorOutR:
	        oled.print("Out Red: ");
            break;
        case ConfigValue::ColorOutG:
	        oled.print("Out Green: ");
            break;
        case ConfigValue::ColorOutB:
	        oled.print("Out Blue: ");
            break;
        case ConfigValue::ColorInR:
	        oled.print("In Red: ");
            break;
        case ConfigValue::ColorInG:
	        oled.print("In Green: ");
            break;
        case ConfigValue::ColorInB:
	        oled.print("In Blue: ");
            break;
        default:
            oled.print("Unknown cfg option: ");
            break;
    }
	oled.print(macropad.get_encoder_position());
}

void printKeys(SH1106_SPI oled)
{
	const char* const labels[4][3] = {
		{"Back", "Save", "Bright"},
		{"Out R", "Out G", "Out B"},
		{"In R", "In G", "In B"},
		{"", "", ""},
	};

	for (uint8_t y = 0; y < 4; y++) {
		for (uint8_t x = 0; x < 3; x++) {
			oled.gotoXY(x * 43, y * 2);
			oled.print(labels[y][x]);
		}
	}
}

template <ConfigValue val>
void switch_active(bool rising, bool falling)
{
    (void) falling;

    if (rising) {
        if (activeValue == val) {
            activeValue = ConfigValue::None;
        } else {
            activeValue = val;
        }

        Macropad& macropad = Macropad::get_instance();
        CFG* cfg = Config::get();

        switch (activeValue) {
            case ConfigValue::Brightness:
                macropad.set_encoder_position(cfg->brightness);
                break;
            case ConfigValue::ColorOutR:
                macropad.set_encoder_position(cfg->colorOutR);
                break;
            case ConfigValue::ColorOutG:
                macropad.set_encoder_position(cfg->colorOutG);
                break;
            case ConfigValue::ColorOutB:
                macropad.set_encoder_position(cfg->colorOutB);
                break;
            case ConfigValue::ColorInR:
                macropad.set_encoder_position(cfg->colorInR);
                break;
            case ConfigValue::ColorInG:
                macropad.set_encoder_position(cfg->colorInG);
                break;
            case ConfigValue::ColorInB:
                macropad.set_encoder_position(cfg->colorInB);
                break;
            case ConfigValue::None:
                macropad.set_encoder_position(0);
                break;
        }
    }
}

} // namespace ConfigState
