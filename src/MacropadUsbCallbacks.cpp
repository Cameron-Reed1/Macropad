#include <tusb.h>
#include <bsp/board.h>
#include <tusb_types.h>
#include <device/usbd.h>
#include <pico/bootrom.h>
#include "usb_descriptors.h"
#include <hardware/watchdog.h>
#include <pico/usb_reset_interface.h>

#include "Macropad.h"
#include "PinDefs.h"

class SuspendedState: public MacropadState
{
public:
    SuspendedState();

    void KeyAny(uint8_t key, bool rising, bool falling);

public:
    bool wakeup;
};

SuspendedState::SuspendedState()
    : MacropadState(nullptr) { }

void SuspendedState::KeyAny(uint8_t key, bool rising, bool falling)
{
    if (falling && wakeup) {
        tud_remote_wakeup();
    }
}

static SuspendedState suspendedState = SuspendedState();

void resume(uint8_t key, bool rising, bool falling)
{
	(void) key;
	(void) rising;

	if (falling) {
		tud_remote_wakeup();
    }
}

void tud_mount_cb(void) { }

void tud_umount_cb(void) { }

void tud_suspend_cb(bool remote_wakeup_en) {
    suspendedState.wakeup = remote_wakeup_en;
	suspendedState.set_parent_state(Macropad::get_instance().get_macropad_state());
    suspendedState.Activate();
    suspendedState.set_pixels_brightness(5);
	suspendedState.set_pixel_color(1, Macropad::get_instance().pixelColor(0, 255 * (remote_wakeup_en ? 1 : 0), 0));
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


// Copied from https://github.com/raspberrypi/pico-sdk/src/rp2_common/pico_stdio_usb/reset_interface.c
bool tud_vendor_control_xfer_cb(uint8_t rhport, uint8_t stage, const tusb_control_request_t* request)
{
    (void) rhport;

    // nothing to do with DATA & ACK stage
    if (stage != CONTROL_STAGE_SETUP) return true;


    if (request->bRequest == RESET_REQUEST_BOOTSEL) {
        reset_usb_boot(1u << LED, request->wValue & 0x7f);
        // does not return, otherwise we'd return true
    }

    if (request->bRequest == RESET_REQUEST_FLASH) {
        watchdog_reboot(0, 0, 100);
        return true;
    }

    return false;
}

