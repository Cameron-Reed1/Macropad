#include "Macropad.h"

MacropadState suspendedState = MacropadState();

void resume(uint8_t key, bool rising, bool falling) {
	(void) key;
	(void) rising;

	if (falling)
		tud_remote_wakeup();
}

void tud_mount_cb(void) { }

void tud_umount_cb(void) { }

void tud_suspend_cb(bool remote_wakeup_en) {
    if (remote_wakeup_en) {
		suspendedState.set_key_generic_callback(resume);
	}
	suspendedState.set_parent_state(Macropad::get_instance().get_macropad_state());
	Macropad::get_instance().set_macropad_state(&suspendedState);
	Macropad::get_instance().set_pixels_brightness(5);
	Macropad::get_instance().set_pixel_color(1, 0, 255 * remote_wakeup_en, 0);
}

void tud_resume_cb(void) {
	Macropad::get_instance().load_parent_state();
}

uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen) {
    (void) instance;
	(void) report_id;
	(void) report_type;
	(void) buffer;
	(void) reqlen;

    return 0;
}

void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize) {
    (void) instance;

	if (report_type == HID_REPORT_TYPE_OUTPUT) {
		if (report_id == REPORT_ID_KEYBOARD) {
			if ( bufsize < 1 ) return;

			Macropad::get_instance().kbd_leds = buffer[0];
		}
	}
}
