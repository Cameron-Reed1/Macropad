#include <cstdint>
#include <tusb.h>
#include <bsp/board.h>
#include <device/usbd.h>
#include <pico/bootrom.h>
#include "usb_descriptors.h"
#include <common/tusb_types.h>
#include <hardware/watchdog.h>
#include <pico/usb_reset_interface.h>

#include "Macropad.h"
#include "PinDefs.h"
#include "Config.h"


enum VENDOR_REQ_OUT {
    VENDOR_REQ_OUT_SET_ID = 0x01,
    VENDOR_REQ_OUT_SET_CFG,
};

enum VENDOR_REQ_IN {
    VENDOR_REQ_IN_GET_CMD_NUM = 0x01,
    VENDOR_REQ_IN_GET_CMDS,
};


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

void tud_suspend_cb(bool remote_wakeup_en)
{
    suspendedState.wakeup = remote_wakeup_en;
    suspendedState.set_parent_state(Macropad::get_instance().get_macropad_state());
    suspendedState.Activate();
    suspendedState.set_pixels_brightness(5);
    suspendedState.set_pixel_color(1, Macropad::get_instance().pixelColor(0, 255 * (remote_wakeup_en ? 1 : 0), 0));
}

void tud_resume_cb(void)
{
    Macropad::get_instance().load_parent_state();
}

uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
    (void) instance;
    (void) report_id;
    (void) report_type;
    (void) buffer;
    (void) reqlen;

    return 0;
}

void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
    (void) instance;

    if (report_type == HID_REPORT_TYPE_OUTPUT) {
        if (report_id == REPORT_ID_KEYBOARD) {
            if ( bufsize < 1 ) return;

            Macropad::get_instance().kbd_leds = buffer[0];
        }
    }
}


bool reset_control_cb(uint8_t stage, const tusb_control_request_t* request)
{
    // nothing to do with DATA & ACK stage
    if (stage != CONTROL_STAGE_SETUP) return true;
    if (request->bmRequestType_bit.direction == TUSB_DIR_IN) return true;


    switch (request->bRequest) {
    case RESET_REQUEST_BOOTSEL:
        reset_usb_boot(1u << LED, request->wValue & 0x7f);
        return true; // Unreachable
    case RESET_REQUEST_FLASH:
        watchdog_reboot(0, 0, 100);
        return true;
    }

    return false;
}


bool vendor_in_control_cb(uint8_t rhport, uint8_t stage, const tusb_control_request_t* request)
{
    Macropad& macropad = Macropad::get_instance();
    uint8_t num;

    switch (request->bRequest) {
    case VENDOR_REQ_IN_GET_CMD_NUM:
        if (stage == CONTROL_STAGE_SETUP) {
            return tud_control_xfer(rhport, request, &macropad.VendorCmdCount, sizeof(macropad.VendorCmdCount));
        } else {
            return true;
        }
    case VENDOR_REQ_IN_GET_CMDS:
        if (stage == CONTROL_STAGE_SETUP) {
            if (request->wLength % sizeof(uint32_t) == 0
                    && request->wLength <= sizeof(macropad.VendorCmds[0]) * macropad.VendorCmdCount) {
                return tud_control_xfer(rhport, request, macropad.VendorCmds, request->wLength);
            }
        } else if (stage == CONTROL_STAGE_DATA) {
            if (request->wLength % sizeof(uint32_t) == 0) {
                macropad.clear_vendor_cmds(request->wLength / sizeof(uint32_t));
                return true;
            }
        } else {
            return true;
        }
    }

    return false;
}


bool vendor_out_control_cb(uint8_t stage, const tusb_control_request_t* request)
{
    // nothing to do with DATA & ACK stage
    if (stage != CONTROL_STAGE_SETUP) return true;


    Macropad& macropad = Macropad::get_instance();

    switch (request->bRequest) {
    case VENDOR_REQ_OUT_SET_ID:
        if (macropad.ComputerID != request->wValue) {
            macropad.ComputerID = request->wValue;
            macropad.update_oled();
        }
        return true;
    case VENDOR_REQ_OUT_SET_CFG:
        if (request->wValue < NUM_SLOTS) {
            Config::switchSlot(request->wValue);
            macropad.update_oled();
            return true;
        }
    }

    return false;
}


bool tud_vendor_control_xfer_cb(uint8_t rhport, uint8_t stage, const tusb_control_request_t* request)
{
    (void) rhport;

    // The reboot interface uses the class type
    if (request->bmRequestType_bit.type == TUSB_REQ_TYPE_CLASS) {
        return reset_control_cb(stage, request);
    } else if (request->bmRequestType_bit.type == TUSB_REQ_TYPE_VENDOR) {
        if (request->bmRequestType_bit.direction == TUSB_DIR_IN) {
            // TUSB_DIR_IN, Device to host
            return vendor_in_control_cb(rhport, stage, request);
        } else {
            // TUSB_DIR_OUT, Host to device
            return vendor_out_control_cb(stage, request);
        }
    }

    return false;
}

