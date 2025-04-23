#include <cstdint>
#include <pico/stdlib.h>
#include <pico/binary_info.h>
#include <hardware/pwm.h>
#include <hardware/i2c.h>
#include <hardware/spi.h>
#include <hardware/watchdog.h>
#include <tusb.h>
#include <bsp/board.h>
#include "usb_descriptors.h"
#include "Macropad.h"
#include "PinDefs.h"
#include "Keys.h"

uint8_t const ascii_to_keycode_conv[128][2] =  { HID_ASCII_TO_KEYCODE };
Macropad Macropad::s_Instance;

Macropad::Macropad()
    : m_Keys(NUM_KEYS, true), m_Oled(SPI_PORT), m_State(nullptr), m_Encoder(ROTA, ROTB, RotaryEncoder::LatchMode::FOUR3),
    m_Pixels(NUM_PIXELS, NEOPIXEL, NEO_GRB + NEO_KHZ800)
{}

Macropad& Macropad::get_instance()
{
    return s_Instance;
}

void Macropad::init(bool init_tinyUSB, bool init_keys, bool init_oled, bool init_speaker, bool init_pixels, bool init_encoder, bool init_stemma)
{
    if (init_tinyUSB) {
        this->init_tinyUSB();
    }

    if (init_keys) {
        this->init_keys();
    }

    if (init_speaker) {
        this->init_speaker();
    }

    if (init_encoder) {
        this->init_encoder();
    }

    if (init_oled) {
        this->init_oled();
    }

    if (init_pixels) {
        this->init_pixels();
    }

    if (init_stemma) {
        this->init_stemma();
    }
}

void Macropad::init_tinyUSB()
{
    board_init();
    tusb_init();
    m_UseTinyUSB = true;
}

void Macropad::init_keys()
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

    m_UseKeys = true;
}

void Macropad::init_oled()
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

    m_Oled.begin();

    m_UseOled = true;
}

void Macropad::init_speaker()
{
    bi_decl_if_func_used(bi_2pins_with_names(SPEAKER, "Speaker", SPEAKER_ENABLE, "Speaker enable"));

    gpio_init(SPEAKER_ENABLE);
    gpio_init(SPEAKER);

    gpio_set_dir(SPEAKER_ENABLE, GPIO_OUT);
    gpio_set_dir(SPEAKER, GPIO_OUT);

    gpio_set_function(SPEAKER, GPIO_FUNC_PWM);

    m_SpeakerSliceNum = pwm_gpio_to_slice_num(SPEAKER);
    m_SpeakerSliceChan = pwm_gpio_to_channel(SPEAKER);
    pwm_config cfg = pwm_get_default_config();
    pwm_config_set_clkdiv(&cfg, 133);
    pwm_config_set_wrap(&cfg, (uint16_t) (MEGAHERTZ / 5));
    pwm_init(m_SpeakerSliceNum, &cfg, false);

    m_UseSpeaker = true;
}


void Macropad::init_pixels()
{
    bi_decl_if_func_used(bi_1pin_with_name(NEOPIXEL, "Neopixel"));

    gpio_init(LED);
    gpio_init(NEOPIXEL);

    gpio_set_dir(LED, GPIO_OUT);
    gpio_set_dir(NEOPIXEL, GPIO_OUT);

    m_Pixels.begin();
    m_Pixels.setBrightness(m_State->get_pixels_brightness());
    for (uint8_t i = 0; i < NUM_PIXELS; i++) {
        m_Pixels.setPixelColor(i, m_State->get_pixel_color(i));
    }
    m_Pixels.show();

    m_UsePixels = true;
}

void Macropad::init_encoder()
{
    bi_decl_if_func_used(bi_3pins_with_names(ENCODER_BUTTON, "Rotary encoder press", ROTA, "Rotary encoder A", ROTB, "Rotary encoder B"));

    gpio_init(ENCODER_BUTTON);
    gpio_init(ROTA);
    gpio_init(ROTB);

    gpio_set_dir(ENCODER_BUTTON, GPIO_IN);

    gpio_pull_up(ENCODER_BUTTON);

    gpio_set_irq_enabled_with_callback(ROTA, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    gpio_set_irq_enabled(ROTB, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);

    m_Encoder.tick();

    m_UseEncoder = true;
}

void Macropad::init_stemma()
{
    bi_decl_if_func_used(bi_2pins_with_names(STEMMA_SDA, "Stemma data", STEMMA_SCL, "Stemma clock"));

    gpio_init(STEMMA_SDA);
    gpio_init(STEMMA_SCL);

    gpio_set_dir(STEMMA_SDA, GPIO_OUT);
    gpio_set_dir(STEMMA_SCL, GPIO_OUT);

    m_UseStemma = true;
}

MacropadState* Macropad::get_macropad_state()
{
    return m_State;
}

bool Macropad::get_key_state(uint8_t key)
{
    if (!m_UseKeys) {
        return false;
    }

    return m_Keys.getKeyValue(key);
}

int Macropad::get_encoder_position()
{
    if (!m_UseEncoder) {
        return 0;
    }

    return m_State->get_encoder_position();
}

void Macropad::set_encoder_position(int new_position)
{
    if (!m_UseEncoder) {
        return;
    }

    m_State->set_encoder_position(new_position);
    m_Encoder.setPosition(new_position);
}

void Macropad::run()
{
    m_StateChanged = false;

    if (m_UseEncoder) {
        run_encoder();
    }

    if (m_UseKeys) {
        run_keys();
    }

    if (m_UsePixels && m_Pixels.canShow()) {
        run_pixels();
    }

    if (m_UseOled && m_UpdateOled) {
        run_oled();
    }

    if (m_UseTinyUSB) {
        run_tinyUSB();
    }

    sleep_ms(10);
}

void Macropad::run_encoder()
{
    m_State->set_encoder_position(m_Encoder.getPosition());
    m_EncoderPressed = !gpio_get(ENCODER_BUTTON);
    m_State->EncoderHandler();

    if (m_EncoderPressed != m_EncoderWasPressed && !m_StateChanged) {
        m_State->EncoderPress(m_EncoderPressed, m_EncoderWasPressed);
        m_UpdateOled = m_UpdateOled || m_State->get_update_on_encoder_press();
    }

    if (m_State->get_encoder_position() != m_State->get_encoder_last_postition() && !m_StateChanged) {
        m_UpdateOled = m_UpdateOled || m_State->get_update_on_encoder_move();
    }

    m_EncoderWasPressed = m_EncoderPressed;
}

void Macropad::run_keys()
{
    m_Keys.update();

    if (m_Keys.haveChanged()) {
        m_UpdateOled = m_UpdateOled || m_State->get_update_on_key_press();

        for (uint8_t i = 0; i < NUM_KEYS; i++) {
            bool rising = m_Keys.getKeyRisingEdge(i);
            bool falling = m_Keys.getKeyFallingEdge(i);

            if (rising || falling) {
                m_State->Key(i, rising, falling);
            }
        }
    }
}

void Macropad::run_pixels()
{
    for (uint8_t i = 0; i < NUM_PIXELS; i++) {
        m_Pixels.setPixelColor(i, m_State->get_pixel_color(i));
    }

    if (m_Pixels.getBrightness() != m_State->get_pixels_brightness()) {
        m_Pixels.setBrightness(m_State->get_pixels_brightness());
    }

    m_Pixels.show();
}

void Macropad::run_oled()
{
    m_Oled.clear();
    m_State->OledDraw(m_Oled);
    m_UpdateOled = false;
}

void Macropad::run_tinyUSB()
{
    tud_task();
    while (!tud_hid_ready() && (m_KeysPressed[0] != HID_KEY_NONE || m_ConsumerReportQueued || m_SystemReportQueued || m_RunningMacro) && tud_mounted()) {
        tud_task();
    }

    if (tud_hid_ready()) {
        if (m_RunningMacro) {
            run_macroStep();
        } else if (m_SystemReportQueued) {
            tud_hid_report(REPORT_ID_SYSTEM_CONTROL, &m_SystemReport, sizeof(m_SystemReport));
            m_SystemReportQueued = false;
        } else if (m_ConsumerReportQueued) {
            tud_hid_report(REPORT_ID_CONSUMER_CONTROL, &m_ConsumerReport, sizeof(m_ConsumerReport));
            m_ConsumerReportQueued = false;
        } else {
            tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, m_KeysPressed);
        }
    }
}

void Macropad::run_macroStep()
{
    // Clear all pressed keys
    if (m_KeysPressed[0] != HID_KEY_NONE) {
        for (uint8_t i = 0; i < 6; i++) {
            m_KeysPressed[i] = HID_KEY_NONE;
            m_TimesPressed[i] = 0;
        }
        tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, m_KeysPressed);
        return;
    }

    MacroStep& step = m_RunningMacro->getStep();
    uint8_t* keys;
    switch (step.step_type) {
        case TYPE_KEY_PRESS:
            keys = reinterpret_cast<uint8_t*>(&step.data);
            tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, keys);
            while (!tud_hid_ready()) {
                tud_task();
            }
            tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, m_KeysPressed);
            m_RunningMacro->nextStep();
            break;
        case TYPE_TYPE_STRING:
            this->type(reinterpret_cast<char*>(step.data));
            m_RunningMacro->nextStep();
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
                m_RunningMacro->nextStep();
            }
    }

    if (m_RunningMacro->finished()) {
        m_RunningMacro = nullptr;
    }
}

void Macropad::encoder_tick()
{
    m_Encoder.tick();
}

void Macropad::update_oled()
{
    if (!m_UseOled) {
        return;
    }

    m_UpdateOled = true;
}

void Macropad::set_macropad_state(MacropadState* newState)
{
    m_State = newState;
    m_UpdateOled = true;
    m_Encoder.setPosition(m_State->get_encoder_position());
    m_StateChanged = true;

    for (uint8_t i = 0; i < 6; i++) {
        m_KeysPressed[i] = HID_KEY_NONE;
        m_TimesPressed[i] = 0;
    }
}

void Macropad::load_parent_state()
{
    MacropadState* parent = m_State->get_parent_state();
    if (parent != nullptr) {
        this->set_macropad_state(parent);
    }
}

uint32_t Macropad::pixelColor(uint8_t red, uint8_t green, uint8_t blue)
{
    return m_Pixels.Color(red, green, blue);
}

void Macropad::type(const char *str)
{
    if (!m_UseTinyUSB || !tud_mounted()) {
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

bool Macropad::press_key(uint8_t key)
{
    if (!m_UseTinyUSB) {
        return false;
    }

    for (uint8_t i = 0; i < 6; i++) {
        if (m_KeysPressed[i] == key) {
            m_TimesPressed[i] += 1;
            return true;
        }
    }

    for (uint8_t i = 0; i < 6; i++) {
        if (m_KeysPressed[i] == HID_KEY_NONE) {
            m_KeysPressed[i] = key;
            m_TimesPressed[i] = 1;
            return true;
        }
    }

    return false;
}

bool Macropad::release_key(uint8_t key)
{
    if (!m_UseTinyUSB) {
        return false;
    }

    for (uint8_t i = 0; i < 6; i++) {
        if (m_KeysPressed[i] == key) {
            if (m_TimesPressed[i] > 1) {
                m_TimesPressed[i] -= 1;
            } else {
                while (m_KeysPressed[i + 1] && i < 5) {
                    m_KeysPressed[i] = m_KeysPressed[i + 1];
                    m_TimesPressed[i] = m_TimesPressed[i + 1];
                    i++;
                }
                m_KeysPressed[i] = HID_KEY_NONE;
                m_TimesPressed[i] = 0;
            }

            return true;
        }
    }

    return false;
}

bool Macropad::press_consumer_key(uint16_t key)
{
    if (!m_UseTinyUSB) {
        return false;
    }

    m_ConsumerReport = key;
    m_ConsumerReportQueued = true;
    return true;
}

bool Macropad::release_consumer_keys()
{
    if (!m_UseTinyUSB) {
        return false;
    }

    m_ConsumerReport = 0;
    m_ConsumerReportQueued = true;
    return true;
}

bool Macropad::press_system_key(uint16_t key)
{
    if (!m_UseTinyUSB) {
        return false;
    }

    m_SystemReport = key;
    m_SystemReportQueued = true;
    return true;
}

bool Macropad::release_system_keys()
{
    if (!m_UseTinyUSB) {
        return false;
    }

    m_SystemReport = 0;
    m_SystemReportQueued = true;
    return true;
}

bool Macropad::play_macro(Macro* macro)
{
    if (m_RunningMacro) {
        return false;
    }

    macro->restart();
    m_RunningMacro = macro;
    return true;
}


bool Macropad::send_vendor_cmd(uint16_t category, uint16_t command)
{
    if (VendorCmdCount >= 6) {
        return false;
    }

    VendorCmds[VendorCmdCount++] = (category << 16) | command;
    return true;
}

void Macropad::clear_vendor_cmds(uint8_t num)
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


void Macropad::gpio_callback(uint gpio, uint32_t events)
{
    (void) gpio;
    (void) events;
    Macropad::get_instance().encoder_tick();
}

