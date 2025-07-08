#include "states/GenericState.h"
#include "states/ConfigState.h"
#include "states/DebugState.h"
#include "states/TeamsState.h"
#include "states/TimerState.h"
#include "states/LinkState.h"

#include "states/MenuState.h"
#include "states/common.h"
#include "MacropadState.h"
#include "ComputerIDs.h"
#include "Macropad.h"
#include "fs.h"


char MenuState::message[22] = "";

MenuState::MenuState()
    : MacropadState(nullptr), teamsState(this), genericState(this), configState(this), linkState(this), debugState(this), timerState(this)
{
    set_oled_automatic_updates(true);
}


void MenuState::KeyAny(uint8_t key, bool rising, bool falling)
{
    ripple(key, rising, falling);
}

void MenuState::Key1(bool rising, bool falling)
{
    (void) rising;

    if (falling) {
        if (Macropad::get_instance().ComputerID == FRAMEWORK_KDE) {
            timerState.Activate();
        } else {
            teamsState.Activate();
        }
    }
}

void MenuState::Key2(bool rising, bool falling)
{
    (void) rising;

    if (falling) {
        genericState.Activate();
    }
}

void MenuState::Key3(bool rising, bool falling)
{
    (void) rising;

    if (falling) {
        configState.Activate();
    }
}

void MenuState::Key4(bool rising, bool falling)
{
    (void) rising;

    if (falling) {
        linkState.Activate();
    }
}

void MenuState::Key5(bool rising, bool falling)
{
    (void) rising;

    if (falling) {
        debugState.Activate();
    }
}


void MenuState::EncoderHandler()
{
    encoder_volume(m_EncoderLastPosition, m_EncoderPosition);
}

void MenuState::EncoderPress(bool rising, bool falling)
{
    toggle_mute(rising, falling);
}


void MenuState::UpdateMessage()
{
    fs::File file("message.txt", LFS_O_RDONLY);
    if (file.open_err < 0) {
        memcpy(message, "Read failed", 12);
        return;
    }

    lfs_size_t len = file.read(message, sizeof(message) - 1);
    if (len < 0) {
        memcpy(message, "Read failed", 12);
        return;
    }
    message[len] = '\x00';

    Macropad::get_instance().update_oled();
}


void MenuState::OledDraw(SH1106_SPI oled)
{
    Macropad& macropad = Macropad::get_instance();

    oled.gotoXY(0, 0);
    oled.print("Encoder Position: ");
    oled.print(macropad.get_encoder_position());

    oled.gotoXY(0, 1);
    oled.print(MenuState::message);

    oled.gotoXY(0, 2);
    if (macropad.ComputerID == FRAMEWORK_KDE) {
        oled.print("Hello, Cameron");
    } else if (macropad.ComputerID != 0) {
        oled.print("Computer ID: ");
        oled.print(macropad.ComputerID);
    }


    /* CFG* cfg = Config::get();
    oled.gotoXY(0, 1);
    oled.print("Config value: ");
    oled.print(cfg->brightness);

    CFG* cfg_saved = Config::savedValues();
    oled.gotoXY(0, 2);
    oled.print("Saved value: ");
    oled.print(cfg_saved->brightness); */

    for (int i = 0; i < NUM_KEYS; i++) {
        if (macropad.get_key_state(i)) {
            oled.gotoXY((i % 3) * 48, (i / 3) + 4);
            oled.print("KEY");
            oled.print(i + 1);
        }
    }
}

