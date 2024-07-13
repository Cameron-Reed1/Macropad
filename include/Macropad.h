#pragma once

#include <pico/stdio.h>
#include <hardware/spi.h>
#include "RotaryEncoder.h"
#include "MacropadState.h"
#include "Adafruit_NeoPixel.hpp"
#include "SH1106_SPI.h"
#include "Macro.h"
#include "Keys.h"

#define SPI_PORT spi1
#define NUM_KEYS 12
#define NUM_PIXELS NUM_KEYS
#define MEGAHERTZ 1000 * 1000

void gpio_callback(uint gpio, uint32_t events);

class Macropad
{
public:
    void init(bool init_tinyUSB, bool init_keys, bool init_oled, bool init_speaker, bool init_pixels, bool init_encoder, bool init_stemma);

    static Macropad& get_instance();
	void run();

	void set_macropad_state(MacropadState* newState);
	MacropadState* get_macropad_state();
	void load_parent_state();

	bool get_key_state(uint8_t key);
    
	int get_encoder_position();
	void set_encoder_position(int new_position);

    uint32_t pixelColor(uint8_t red, uint8_t green, uint8_t blue);

	void update_oled();
	void set_oled_automatic_updates(bool enable = true);
	void set_oled_automatic_updates(bool on_encoder_move, bool on_encoder_press, bool on_key_press);

	void type(const char *str);
	bool press_key(uint8_t key);
	bool release_key(uint8_t key);
	bool press_consumer_key(uint16_t key);
	bool release_consumer_keys();
	bool press_system_key(uint16_t key);
	bool release_system_keys();
	bool play_macro(Macro* macro);

	Macropad(const Macropad&) = delete;

public:
	uint8_t kbd_leds = 0;

private:
	Macropad();

	void encoder_tick();

    static void gpio_callback(uint gpio, uint32_t events);

	void init_tinyUSB();
    void init_keys();
    void init_oled();
    void init_speaker();
    void init_pixels();
    void init_encoder();
    void init_stemma();

    void run_encoder();
    void run_keys();
    void run_pixels();
    void run_oled();
    void run_tinyUSB();
    void run_macroStep();

private:
	static Macropad s_Instance;

	MacropadState* m_State;

	bool m_UseTinyUSB = false;
    bool m_UseKeys = false;
    bool m_UseOled = false;
    bool m_UseSpeaker = false;
    bool m_UsePixels = false;
    bool m_UseEncoder = false;
    bool m_UseStemma = false;

    bool m_UpdateOled = true;

	bool m_StateChanged;

    bool m_EncoderPressed;
	bool m_EncoderWasPressed;

	uint8_t m_KeysPressed[6] = { 0 };
	uint8_t m_TimesPressed[6] = { 0 };

	uint16_t m_ConsumerReport;
	bool m_ConsumerReportQueued;

	uint16_t m_SystemReport;
	bool m_SystemReportQueued;

    uint m_SpeakerSliceNum;
    uint m_SpeakerSliceChan;

	Macro* m_RunningMacro = nullptr;

    Keys m_Keys;
    SH1106_SPI m_Oled;
	RotaryEncoder m_Encoder;
    Adafruit_NeoPixel m_Pixels;
};

