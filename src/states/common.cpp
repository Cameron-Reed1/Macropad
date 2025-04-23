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
    } else if (falling && get_absolute_time() - press_start[key] > 320 * 1000) {
        ripple_anim_run(key, cfg->colorInR, cfg->colorInG, cfg->colorInB, DIR_IN);
    }
}

void return_to_parent_state(bool rising, bool falling)
{
    (void) rising;

    if (falling) {
        macropad::LoadParentState();
    }
}

void encoder_volume(int last_position, int new_position)
{
    static bool key_pressed = false;
    if (new_position - last_position > 0) {
        macropad::PressConsumerKey(HID_USAGE_CONSUMER_VOLUME_INCREMENT);
        key_pressed = true;
    } else if (new_position - last_position < 0) {
        macropad::PressConsumerKey(HID_USAGE_CONSUMER_VOLUME_DECREMENT);
        key_pressed = true;
    } else if (key_pressed) {
        macropad::ReleaseConsumerKeys();
        key_pressed = false;
    }
}

void encoder_brightness(int last_position, int new_position)
{
    if (new_position - last_position > 0) {
        macropad::SendVendorCmd(0x0000, 0x0001);
    } else if (new_position - last_position < 0) {
        macropad::SendVendorCmd(0x0000, 0x0000);
    }
}

void toggle_mute(bool rising, bool falling)
{
    if (rising) {
        macropad::PressConsumerKey(HID_USAGE_CONSUMER_MUTE);
    } else if (falling) {
        macropad::ReleaseConsumerKeys();
    }
}

