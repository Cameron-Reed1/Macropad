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

uint8_t const ascii_to_keycode_conv[128][2] =  { HID_ASCII_TO_KEYCODE };
Macropad Macropad::s_Instance;

Macropad::Macropad()
: m_keys(NUM_KEYS, true), m_oled(SPI_PORT), m_state(nullptr), m_encoder(ROTA, ROTB, RotaryEncoder::LatchMode::FOUR3), m_pixels(NUM_PIXELS, NEOPIXEL, NEO_GRB + NEO_KHZ800)
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
	m_useTinyUSB = true;
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

    m_useKeys = true;
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

    m_oled.begin();

    m_useOled = true;
}

void Macropad::init_speaker()
{
	bi_decl_if_func_used(bi_2pins_with_names(SPEAKER, "Speaker", SPEAKER_ENABLE, "Speaker enable"));

    gpio_init(SPEAKER_ENABLE);
    gpio_init(SPEAKER);

    gpio_set_dir(SPEAKER_ENABLE, GPIO_OUT);
    gpio_set_dir(SPEAKER, GPIO_OUT);

    gpio_set_function(SPEAKER, GPIO_FUNC_PWM);

    m_speakerSliceNum = pwm_gpio_to_slice_num(SPEAKER);
    m_speakerSliceChan = pwm_gpio_to_channel(SPEAKER);
    pwm_config cfg = pwm_get_default_config();
    pwm_config_set_clkdiv(&cfg, 133);
    pwm_config_set_wrap(&cfg, (uint16_t) (MEGAHERTZ / 5));
    pwm_init(m_speakerSliceNum, &cfg, false);

    m_useSpeaker = true;
}


void Macropad::init_pixels()
{
	bi_decl_if_func_used(bi_1pin_with_name(NEOPIXEL, "Neopixel"));

    gpio_init(LED);
    gpio_init(NEOPIXEL);

    gpio_set_dir(LED, GPIO_OUT);
    gpio_set_dir(NEOPIXEL, GPIO_OUT);

    m_pixels.begin();
    m_pixels.setBrightness(m_state->get_pixels_brightness());
    for (uint8_t i = 0; i < NUM_PIXELS; i++) {
        m_pixels.setPixelColor(i, m_state->get_pixel_color(i));
    }
    m_pixels.show();

    m_usePixels = true;
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

    m_encoder.tick();

    m_useEncoder = true;
}

void Macropad::init_stemma()
{
	bi_decl_if_func_used(bi_2pins_with_names(STEMMA_SDA, "Stemma data", STEMMA_SCL, "Stemma clock"));

    gpio_init(STEMMA_SDA);
    gpio_init(STEMMA_SCL);

    gpio_set_dir(STEMMA_SDA, GPIO_OUT);
    gpio_set_dir(STEMMA_SCL, GPIO_OUT);

    m_useStemma = true;
}

MacropadState* Macropad::get_macropad_state()
{
	return m_state;
}

bool Macropad::get_key_state(uint8_t key)
{
	if (!m_useKeys)
		return false;
	return m_keys.getKeyValue(key);
}

int Macropad::get_encoder_position()
{
	if (!m_useEncoder)
		return 0;
	return m_state->get_encoder_position();
}

void Macropad::set_encoder_position(int new_position)
{
	if (!m_useEncoder)
		return;
	m_state->set_encoder_position(new_position);
	m_encoder.setPosition(new_position);
}

void Macropad::run()
{
	m_stateChanged = false;

    if (m_useEncoder) {
        run_encoder();
    }

    if (m_useKeys) {
        run_keys();
    }

    if (m_usePixels && m_pixels.canShow()) {
        run_pixels();
	}

    if (m_useOled && m_updateOled) {
        run_oled();
	}

    if (m_useTinyUSB) {
        run_tinyUSB();
	}

	sleep_ms(10);
}

void Macropad::run_encoder()
{
    m_state->set_encoder_position(m_encoder.getPosition());
    m_encoderPressed = !gpio_get(ENCODER_BUTTON);
    m_state->EncoderHandler();

    if (m_encoderPressed != m_encoderWasPressed && !m_stateChanged) {
        m_state->EncoderPress(m_encoderPressed, m_encoderWasPressed);
        m_updateOled = m_updateOled || m_state->get_update_on_encoder_press();
    }

    if (m_state->get_encoder_position() != m_state->get_encoder_last_postition() && !m_stateChanged) {
        m_updateOled = m_updateOled || m_state->get_update_on_encoder_move();
    }

    m_encoderWasPressed = m_encoderPressed;
}

void Macropad::run_keys()
{
    m_keys.update();

    if (m_keys.haveChanged()) {
        m_updateOled = m_updateOled || m_state->get_update_on_key_press();

        for (uint8_t i = 0; i < NUM_KEYS; i++) {
            bool rising = m_keys.getKeyRisingEdge(i + 1);
            bool falling = m_keys.getKeyFallingEdge(i + 1);

            if (rising || falling) {
                m_state->Key(i, rising, falling);
            }
        }
    }
}

void Macropad::run_pixels()
{
    for (uint8_t i = 0; i < NUM_PIXELS; i++) {
        m_pixels.setPixelColor(i, m_state->get_pixel_color(i));
    }

    if (m_pixels.getBrightness() != m_state->get_pixels_brightness()) {
        m_pixels.setBrightness(m_state->get_pixels_brightness());
    }

    m_pixels.show();
}

void Macropad::run_oled()
{
    m_oled.clear();
    m_state->OledDraw(m_oled);
    m_updateOled = false;
}

void Macropad::run_tinyUSB()
{
    tud_task();
    while (!tud_hid_ready() && (m_keysPressed[0] != HID_KEY_NONE || m_consumerReportQueued || m_systemReportQueued || m_running_macro) && tud_mounted()) {
        tud_task();
    }

    if (tud_hid_ready()) {
        if (m_running_macro) {
            run_macroStep();
        } else if (m_systemReportQueued) {
            tud_hid_report(REPORT_ID_SYSTEM_CONTROL, &m_systemReport, sizeof(m_systemReport));
            m_systemReportQueued = false;
        } else if (m_consumerReportQueued) {
            tud_hid_report(REPORT_ID_CONSUMER_CONTROL, &m_consumerReport, sizeof(m_consumerReport));
            m_consumerReportQueued = false;
        } else {
            tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, m_keysPressed);
        }
    }
}

void Macropad::run_macroStep()
{
    // Clear all pressed keys
    if (m_keysPressed[0] != HID_KEY_NONE) {
        for (uint8_t i = 0; i < 6; i++) {
            m_keysPressed[i] = HID_KEY_NONE;
            m_timesPressed[i] = 0;
        }
        tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, m_keysPressed);
        return;
    }

    MacroStep& step = m_running_macro->getStep();
    uint8_t* keys;
    switch (step.m_step_type) {
        case TYPE_KEY_PRESS:
            keys = reinterpret_cast<uint8_t*>(&step.m_data);
            tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, keys);
            while (!tud_hid_ready()) {
                tud_task();
            }
            tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, m_keysPressed);
            m_running_macro->nextStep();
            break;
        case TYPE_TYPE_STRING:
            this->type(reinterpret_cast<char*>(step.m_data));
            m_running_macro->nextStep();
            break;
        case TYPE_SLEEP:
            static bool sleep_started = false;
            static absolute_time_t sleep_start_time;
            if (!sleep_started) {
                sleep_start_time = get_absolute_time();
                sleep_started = true;
            }
            if (absolute_time_diff_us(sleep_start_time, get_absolute_time()) >= step.m_data * 1000) {
                sleep_started = false;
                m_running_macro->nextStep();
            }
    }

    if (m_running_macro->finished()) {
        m_running_macro = nullptr;
    }
}

void Macropad::encoder_tick()
{
    m_encoder.tick();
}

void Macropad::update_oled()
{
	if (!m_useOled)
		return;
	m_updateOled = true;
}

void Macropad::set_macropad_state(MacropadState* newState)
{
	m_state = newState;
	m_updateOled = true;
	m_encoder.setPosition(m_state->get_encoder_position());
	m_stateChanged = true;

	for (uint8_t i = 0; i < 6; i++) {
		m_keysPressed[i] = HID_KEY_NONE;
		m_timesPressed[i] = 0;
	}
}

void Macropad::load_parent_state()
{
    MacropadState* parent = m_state->get_parent_state();
	if (parent != nullptr) {
		this->set_macropad_state(parent);
    }
}

uint32_t Macropad::pixelColor(uint8_t red, uint8_t green, uint8_t blue)
{
    return m_pixels.Color(red, green, blue);
}

void Macropad::type(const char *str)
{
	if (!m_useTinyUSB || !tud_mounted())
		return;
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
	if (!m_useTinyUSB)
		return false;
	for (uint8_t i = 0; i < 6; i++) {
		if (m_keysPressed[i] == key) {
			m_timesPressed[i] += 1;
			return true;
		}
	}
	for (uint8_t i = 0; i < 6; i++) {
		if (m_keysPressed[i] == HID_KEY_NONE) {
			m_keysPressed[i] = key;
			m_timesPressed[i] = 1;
			return true;
		}
	}
	return false;
}

bool Macropad::release_key(uint8_t key)
{
	if (!m_useTinyUSB)
		return false;
	for (uint8_t i = 0; i < 6; i++) {
		if (m_keysPressed[i] == key) {
			if (m_timesPressed[i] > 1) {
				m_timesPressed[i] -= 1;
			} else {
				while (m_keysPressed[i + 1] && i < 5) {
					m_keysPressed[i] = m_keysPressed[i + 1];
					m_timesPressed[i] = m_timesPressed[i + 1];
					i++;
				}
				m_keysPressed[i] = HID_KEY_NONE;
				m_timesPressed[i] = 0;
			}
			return true;
		}
	}
	return false;
}

bool Macropad::press_consumer_key(uint16_t key)
{
	if (!m_useTinyUSB)
		return false;
	m_consumerReport = key;
	m_consumerReportQueued = true;
	return true;
}

bool Macropad::release_consumer_keys()
{
	if (!m_useTinyUSB)
		return false;
	m_consumerReport = 0;
	m_consumerReportQueued = true;
	return true;
}

bool Macropad::press_system_key(uint16_t key)
{
	if (!m_useTinyUSB)
		return false;
	m_systemReport = key;
	m_systemReportQueued = true;
	return true;
}

bool Macropad::release_system_keys()
{
	if (!m_useTinyUSB)
		return false;
	m_systemReport = 0;
	m_systemReportQueued = true;
	return true;
}

bool Macropad::play_macro(Macro* macro)
{
	if (m_running_macro)
		return false;
	macro->restart();
	m_running_macro = macro;
	return true;
}


void Macropad::gpio_callback(uint gpio, uint32_t events)
{
    (void) gpio;
    (void) events;
    Macropad::get_instance().encoder_tick();
}

