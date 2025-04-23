#include <cstdint>
#include <pico/stdlib.h>
#include <pico/binary_info.h>
#include <hardware/pwm.h>
#include <hardware/i2c.h>
#include <hardware/spi.h>
#include <hardware/watchdog.h>
#include <pico/time.h>
#include <tusb.h>
#include <bsp/board.h>
#include "class/hid/hid.h"
#include "usb_descriptors.h"
#include "Macropad.h"
#include "PinDefs.h"
#include "Keys.h"


uint8_t const ascii_to_keycode_conv[128][2] =  { HID_ASCII_TO_KEYCODE };


namespace macropad {

MacropadState* state;
SH1106_SPI Oled(SPI_PORT);
Adafruit_NeoPixel Pixels(NUM_PIXELS, NEOPIXEL, NEO_GRB + NEO_KHZ800);
RotaryEncoder Encoder(ROTA, ROTB, RotaryEncoder::LatchMode::FOUR3);
Key EncoderBtn(ENCODER_BUTTON);
Key Keys[NUM_KEYS] = { Key(KEY1),  Key(KEY2),  Key(KEY3),
                       Key(KEY4),  Key(KEY5),  Key(KEY6),
                       Key(KEY7),  Key(KEY8),  Key(KEY9),
                       Key(KEY10), Key(KEY11), Key(KEY12) };

uint32_t VendorCmds[VENDOR_CMD_QUEUE_LEN] = { 0 };
uint8_t VendorCmdCount = 0;
uint8_t KeysPressed[6] = { 0 };
uint8_t TimesPressed[6] = { 0 };

uint16_t ConsumerReport = 0;
bool ConsumerReportQueued = 0;

uint16_t SystemReport = 0;
bool SystemReportQueued = false;
Macro* RunningMacro = nullptr;
uint32_t ComputerID = 0;

bool use_encoder = false;
bool use_usb = false;
bool use_keys = false;
bool use_oled = false;
bool use_pixels = false;

bool update_oled = false;

void gpio_callback(uint gpio, uint32_t events);


void init_usb()
{
    board_init();
    tusb_init();

    use_usb = true;
}

void init_keys()
{
    bi_decl_if_func_used(bi_pin_mask_with_name(0x0fff << KEY1, "Keys"));

    gpio_init(KEY1);
    gpio_init(KEY2);
    gpio_init(KEY3);
    gpio_init(KEY4);
    gpio_init(KEY5);
    gpio_init(KEY6);
    gpio_init(KEY7);
    gpio_init(KEY8);
    gpio_init(KEY9);
    gpio_init(KEY10);
    gpio_init(KEY11);
    gpio_init(KEY12);

    gpio_set_dir(KEY1, GPIO_IN);
    gpio_set_dir(KEY2, GPIO_IN);
    gpio_set_dir(KEY3, GPIO_IN);
    gpio_set_dir(KEY4, GPIO_IN);
    gpio_set_dir(KEY5, GPIO_IN);
    gpio_set_dir(KEY6, GPIO_IN);
    gpio_set_dir(KEY7, GPIO_IN);
    gpio_set_dir(KEY8, GPIO_IN);
    gpio_set_dir(KEY9, GPIO_IN);
    gpio_set_dir(KEY10, GPIO_IN);
    gpio_set_dir(KEY11, GPIO_IN);
    gpio_set_dir(KEY12, GPIO_IN);

    gpio_pull_up(KEY1);
    gpio_pull_up(KEY2);
    gpio_pull_up(KEY3);
    gpio_pull_up(KEY4);
    gpio_pull_up(KEY5);
    gpio_pull_up(KEY6);
    gpio_pull_up(KEY7);
    gpio_pull_up(KEY8);
    gpio_pull_up(KEY9);
    gpio_pull_up(KEY10);
    gpio_pull_up(KEY11);
    gpio_pull_up(KEY12);

    use_keys = true;
}

void init_oled()
{
    bi_decl_if_func_used(bi_3pins_with_names(PIN_CS, "Oled Chip Select", PIN_RESET, "Oled reset", PIN_DC, "Oled Data/Command"));
    bi_decl_if_func_used(bi_3pins_with_names(PIN_SCK, "Oled clock", PIN_MOSI, "Oled MOSI", PIN_MISO, "Oled MISO"));

    gpio_init(PIN_CS);
    gpio_init(PIN_RESET);
    gpio_init(PIN_DC);

    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_set_dir(PIN_RESET, GPIO_OUT);
    gpio_set_dir(PIN_DC, GPIO_OUT);

    gpio_put(PIN_CS, 1);

    Oled.begin();

    use_oled = true;
}

void init_encoder()
{
    bi_decl_if_func_used(bi_3pins_with_names(ENCODER_BUTTON, "Rotary encoder press", ROTA, "Rotary encoder A", ROTB, "Rotary encoder B"));

    gpio_init(ENCODER_BUTTON);
    gpio_init(ROTA);
    gpio_init(ROTB);

    gpio_set_dir(ENCODER_BUTTON, GPIO_IN);

    gpio_pull_up(ENCODER_BUTTON);

    gpio_set_irq_enabled_with_callback(ROTA, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    gpio_set_irq_enabled(ROTB, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);

    Encoder.tick();

    use_encoder = true;
}

void init_pixels()
{
    bi_decl_if_func_used(bi_1pin_with_name(NEOPIXEL, "Neopixel"));

    gpio_init(LED);
    gpio_init(NEOPIXEL);

    gpio_set_dir(LED, GPIO_OUT);
    gpio_set_dir(NEOPIXEL, GPIO_OUT);

    Pixels.begin();
    Pixels.setBrightness(state->get_pixels_brightness());
    for (uint8_t i = 0; i < NUM_PIXELS; i++) {
        Pixels.setPixelColor(i, state->get_pixel_color(i));
    }
    Pixels.show();

    use_pixels = true;
}

void init(bool tinyUSB, bool keys, bool encoder, bool pixels, bool oled)
{
    if (tinyUSB) {
        init_usb();
    }

    if (keys) {
        init_keys();
    }

    if (encoder) {
        init_encoder();
    }

    if (pixels) {
        init_pixels();
    }

    if (oled) {
        init_oled();
    }
}

void tick_macroStep()
{
    // Clear all pressed keys
    if (KeysPressed[0] != HID_KEY_NONE) {
        for (uint8_t i = 0; i < 6; i++) {
            KeysPressed[i] = HID_KEY_NONE;
            TimesPressed[i] = 0;
        }
        tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, KeysPressed);
        return;
    }

    MacroStep& step = RunningMacro->getStep();
    uint8_t* keys;
    switch (step.step_type) {
        case TYPE_KEY_PRESS:
            keys = reinterpret_cast<uint8_t*>(&step.data);
            tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, keys);
            while (!tud_hid_ready()) {
                tud_task();
            }
            tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, KeysPressed);
            RunningMacro->nextStep();
            break;
        case TYPE_TYPE_STRING:
            Type(reinterpret_cast<char*>(step.data));
            RunningMacro->nextStep();
            break;
        case TYPE_SLEEP:
            static bool sleep_started = false;
            static absolute_time_t sleep_start_time;
            if (!sleep_started) {
                sleep_start_time = get_absolute_time();
                sleep_started = true;
            }
            if (absolute_time_diff_us(sleep_start_time, get_absolute_time()) >= step.data * 1000) {
                sleep_started = false;
                RunningMacro->nextStep();
            }
    }

    if (RunningMacro->finished()) {
        RunningMacro = nullptr;
    }
}

void tick_usb()
{
    tud_task();
    while (!tud_hid_ready() && (KeysPressed[0] != HID_KEY_NONE || ConsumerReportQueued || SystemReportQueued || RunningMacro) && tud_mounted()) {
        tud_task();
    }

    if (tud_hid_ready()) {
        if (RunningMacro) {
            tick_macroStep();
        } else if (SystemReportQueued) {
            tud_hid_report(REPORT_ID_SYSTEM_CONTROL, &SystemReport, sizeof(SystemReport));
            SystemReportQueued = false;
        } else if (ConsumerReportQueued) {
            tud_hid_report(REPORT_ID_CONSUMER_CONTROL, &ConsumerReport, sizeof(ConsumerReport));
            ConsumerReportQueued = false;
        } else {
            tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, KeysPressed);
        }
    }
}

void tick_encoder()
{
    EncoderBtn.Update();
    state->set_encoder_position(Encoder.getPosition());
    state->EncoderHandler();

    bool rising = EncoderBtn.RisingEdge();
    bool falling = EncoderBtn.FallingEdge();

    if (rising || falling) {
        state->EncoderPress(rising, falling);
        update_oled = update_oled || state->get_update_on_encoder_press();
    }

    if (Encoder.getDirection() != RotaryEncoder::Direction::NOROTATION) {
        update_oled = update_oled || state->get_update_on_encoder_move();
    }
}

void tick_keys()
{
    for (uint8_t i = 0; i < NUM_KEYS; i++) {
        Key& key = Keys[i];
        key.Update();

        bool rising = key.RisingEdge();
        bool falling = key.FallingEdge();

        if (rising || falling) {
            state->Key(i, rising, falling);
            update_oled = update_oled || state->get_update_on_key_press();
        }
    }
}

void tick_oled()
{
    Oled.clear();
    state->OledDraw(Oled);
    update_oled = false;
}

void tick_pixels()
{
    for (uint8_t i = 0; i < NUM_PIXELS; i++) {
        Pixels.setPixelColor(i, state->get_pixel_color(i));
    }

    if (Pixels.getBrightness() != state->get_pixels_brightness()) {
        Pixels.setBrightness(state->get_pixels_brightness());
    }

    Pixels.show();
}

void tick()
{
    if (use_encoder) {
        tick_encoder();
    }

    if (use_keys) {
        tick_keys();
    }

    if (use_usb) {
        tick_usb();
    }

    if (use_oled && update_oled) {
        tick_oled();
    }

    if (use_pixels && Pixels.canShow()) {
        tick_pixels();
    }

    sleep_ms(10);
}

void LoadState(MacropadState* newState)
{
    state = newState;
    update_oled = true;
    Encoder.setPosition(state->get_encoder_position());

    for (uint8_t i = 0; i < 6; i++) {
        KeysPressed[i] = HID_KEY_NONE;
        TimesPressed[i] = 0;
    }
}

void LoadParentState()
{
    MacropadState* parent = state->get_parent_state();
    if (parent) {
        LoadState(parent);
    }
}

bool PressKey(uint8_t key)
{
    if (!use_usb) {
        return false;
    }

    for (uint8_t i = 0; i < 6; i++) {
        if (KeysPressed[i] == key) {
            TimesPressed[i] += 1;
            return true;
        }
    }

    for (uint8_t i = 0; i < 6; i++) {
        if (KeysPressed[i] == HID_KEY_NONE) {
            KeysPressed[i] = key;
            TimesPressed[i] = 1;
            return true;
        }
    }

    return false;
}

bool ReleaseKey(uint8_t key)
{
    if (!use_usb) {
        return false;
    }

    for (uint8_t i = 0; i < 6; i++) {
        if (KeysPressed[i] == key) {
            if (TimesPressed[i] > 1) {
                TimesPressed[i] -= 1;
            } else {
                while (KeysPressed[i + 1] && i < 5) {
                    KeysPressed[i] = KeysPressed[i + 1];
                    TimesPressed[i] = TimesPressed[i + 1];
                    i++;
                }
                KeysPressed[i] = HID_KEY_NONE;
                TimesPressed[i] = 0;
            }

            return true;
        }
    }

    return false;
}

bool PressConsumerKey(uint16_t key)
{
    if (!use_usb) {
        return false;
    }

    ConsumerReport = key;
    ConsumerReportQueued = true;
    return true;
}

bool ReleaseConsumerKeys()
{
    return PressConsumerKey(0);
}

bool PressSystemKey(uint16_t key)
{
    if (!use_usb) {
        return false;
    }

    SystemReport = key;
    SystemReportQueued = true;
    return true;
}

bool ReleaseSystemKeys()
{
    return PressSystemKey(0);
}

bool SendVendorCmd(uint16_t category, uint16_t command)
{
    if (VendorCmdCount >= VENDOR_CMD_QUEUE_LEN) {
        return false;
    }

    VendorCmds[VendorCmdCount++] = (category << 16) | command;
    return true;
}

void ClearVendorCmds(uint8_t num)
{
    // Only allow clearing at most VendorCmdCount commands
    if (num > VendorCmdCount) {
        num = VendorCmdCount;
    }

    // Clear the first num commands
    for (int i = 0; i < num; i++) {
        VendorCmds[i] = 0;
    }

    // Move any remaining commands down
    for (int i = num; i < VendorCmdCount; i++) {
        VendorCmds[i - num] = VendorCmds[i];
    }

    VendorCmdCount -= num;
}

bool PlayMacro(Macro* macro)
{
    if (RunningMacro) {
        return false;
    }

    macro->restart();
    RunningMacro = macro;
    return true;
}

void Type(const char* str)
{
    if (!use_usb || !tud_mounted()) {
        return;
    }

    uint8_t report[6] = {0}, modifier = 0, i = 0;
    while (str[i] != 0x00 && i != 255) {
        while (!tud_hid_ready()) {
            tud_task();
        }
        report[0] = ascii_to_keycode_conv[(uint8_t) str[i]][1];
        if (ascii_to_keycode_conv[(uint8_t) str[i]][0]) {
            modifier = KEYBOARD_MODIFIER_LEFTSHIFT;
        } else {
            modifier = 0;
        }
        tud_hid_keyboard_report(REPORT_ID_KEYBOARD, modifier, report);
        while (!tud_hid_ready()) {
            tud_task();
        }
        report[0] = HID_KEY_NONE;
        tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, report);
        i++;
    }
}

void UpdateOled()
{
    update_oled = use_oled;
}

void gpio_callback(uint gpio, uint32_t events)
{
    (void) gpio;
    (void) events;
    Encoder.tick();
}

}; // namespace macropad

