#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "pico/time.h"
#include "states/MenuState.h"
#include "RippleAnim.h"
#include "Macropad.h"
#include "PinDefs.h"
#include "Config.h"
#include "fs.h"
#include "boop.h"

static int boop_index = boop_size;

bool audio_cb(repeating_timer_t* rt);

int main(void)
{
    Macropad& macropad = Macropad::get_instance();
    MenuState menuState = MenuState();
    CFG* cfg = Config::read();
    fs::init();

    MenuState::UpdateMessage();

    macropad.init(true, true, true, false, false, true, false);
    menuState.Activate();
    ripple_anim_setup();
    ripple_anim_set_brightness(cfg->brightness);

    repeating_timer_t timer;
    add_repeating_timer_us(23, audio_cb, nullptr, &timer);

    while (true) {
        macropad.run();
    }

    return 0;
}

void start_boop()
{
    if (boop_index >= boop_size) {
        boop_index = 0;
        gpio_put(SPEAKER_ENABLE, true);
    }
}

bool audio_cb(repeating_timer_t* rt)
{
    (void) rt;
    Macropad& macropad = Macropad::get_instance();
    if (boop_index < boop_size) {
        pwm_set_chan_level(macropad.SpeakerSliceNum, macropad.SpeakerSliceChan, boop[boop_index]);
        boop_index++;
    } else {
        pwm_set_chan_level(macropad.SpeakerSliceNum, macropad.SpeakerSliceChan, 0);
    }

    return true;
}

