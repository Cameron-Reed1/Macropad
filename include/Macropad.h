#pragma once

#include <pico/stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/i2c.h"
#include "hardware/watchdog.h"
#include "hardware/pwm.h"
#include "PinDefs.h"
#include "RotaryEncoder.h"
#include "MacropadState.h"
#include "Keys.h"
#include "Adafruit_NeoPixel.hpp"
#include "SH1106_SPI.h"
#include "bsp/board.h"
#include "tusb.h"
#include "usb_descriptors.h"
#include "Macro.h"

#define SPI_PORT spi1
#define NUM_KEYS 12
#define NUM_PIXELS NUM_KEYS
#define MEGAHERTZ 1000 * 1000

void gpio_callback(uint gpio, uint32_t events);

class Macropad
{
public:
    void init(MacropadState* state); 
    void init(bool init_tinyUSB, bool init_keys, bool init_oled, bool init_speaker, bool init_pixels, bool init_encoder, bool init_stemma);
    void init(MacropadState* state, bool init_tinyUSB, bool init_keys, bool init_oled, bool init_speaker, bool init_pixels, bool init_encoder, bool init_stemma);
	void run();
	int get_encoder_position();
	bool get_key_state(uint8_t key);
	void update_oled();
	void set_oled_automatic_updates(bool enable = true);
	void set_oled_automatic_updates(bool on_encoder_move, bool on_encoder_press, bool on_key_press);
	void encoder_tick();
	void set_encoder_position(int new_position);
	void set_macropad_state(MacropadState* newState);
    void set_key_function(uint8_t key, keyCallback newFunc);
    void set_general_key_function(keyCallbackGeneric newFunc);
    void set_pixel_color(uint8_t pixel, uint8_t red, uint8_t green, uint8_t blue);
	void set_pixels_brightness(uint8_t brightness);
	void load_parent_state();
	void type(const char *str);
	bool press_key(uint8_t key);
	bool release_key(uint8_t key);
	bool press_consumer_key(uint16_t key);
	bool release_consumer_keys();
	bool press_system_key(uint16_t key);
	bool release_system_keys();
	bool run_macro(Macro* macro);
	MacropadState* get_macropad_state();
	Macropad(const Macropad&) = delete;
    static Macropad& get_instance();
	SH1106_SPI get_oled();
public:
	uint8_t kbd_leds = 0;
private:
	Macropad();
	bool m_useTinyUSB = false;
    bool m_useKeys = false;
    bool m_useOled = false;
    bool m_useSpeaker = false;
    bool m_usePixels = false;
    bool m_useEncoder = false;
    bool m_useStemma = false;
    bool m_updateOled = true;
	bool m_stateChanged;
    bool m_encoderPressed;
	bool m_encoderWasPressed;
	bool m_consumerReportQueued;
	bool m_systemReportQueued;
    uint m_speakerSliceNum;
    uint m_speakerSliceChan;
	uint8_t m_keysPressed[6] = { 0 };
	uint8_t m_timesPressed[6] = { 0 };
	uint16_t m_consumerReport;
	uint16_t m_systemReport;
	Macro* m_running_macro = nullptr;
    Keys m_keys;
    SH1106_SPI m_oled;
	MacropadState* m_state;
	RotaryEncoder m_encoder;
    Adafruit_NeoPixel m_pixels;
	void init_tinyUSB();
    void init_keys();
    void init_oled();
    void init_speaker();
    void init_pixels();
    void init_encoder();
    void init_stemma();
	static Macropad s_instance;
};
