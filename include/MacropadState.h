#pragma once

#include <stdint.h>
#include <SH1106_SPI.h>


class MacropadState
{
public:
    MacropadState() = delete;
	MacropadState(MacropadState* parent);

	void set_parent_state(MacropadState* parent);
	MacropadState* get_parent_state();

	void set_pixels_brightness(uint16_t brightness);
	void set_pixel_color(uint8_t pixel, uint32_t color);
	uint16_t get_pixels_brightness();
	uint32_t get_pixel_color(uint8_t pixel);

	void set_encoder_position(int newPos);
	int get_encoder_position();
	int get_encoder_last_postition();

	void set_oled_automatic_updates(bool enable = true);
	void set_oled_automatic_updates(bool on_encoder_move, bool on_encoder_press, bool on_key_press);
	bool get_update_on_encoder_move();
	bool get_update_on_encoder_press();
	bool get_update_on_key_press();

    virtual void Activate();

    void Key(uint8_t key, bool rising, bool falling);

    virtual void KeyAny(uint8_t key, bool rising, bool falling) { };

    virtual void Key1(bool rising, bool falling) { };
    virtual void Key2(bool rising, bool falling) { };
    virtual void Key3(bool rising, bool falling) { };
    virtual void Key4(bool rising, bool falling) { };
    virtual void Key5(bool rising, bool falling) { };
    virtual void Key6(bool rising, bool falling) { };
    virtual void Key7(bool rising, bool falling) { };
    virtual void Key8(bool rising, bool falling) { };
    virtual void Key9(bool rising, bool falling) { };
    virtual void Key10(bool rising, bool falling) { };
    virtual void Key11(bool rising, bool falling) { };
    virtual void Key12(bool rising, bool falling) { };

    virtual void EncoderHandler() { };
    virtual void EncoderPress(bool, bool) { };
    virtual void OledDraw(SH1106_SPI oled) { };

protected:
	MacropadState* m_Parent = nullptr;

    int16_t m_PixelBrightness = 80;
	uint32_t m_PixelColors[12] = {0};

	int m_EncoderPosition = 0;
	int m_EncoderLastPosition = -1;

	bool m_UpdateOnKeyPress = false;
	bool m_UpdateOnEncoderMove = false;
	bool m_UpdateOnEncoderPress = false;
};
