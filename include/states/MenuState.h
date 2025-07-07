#pragma once

#include "states/GenericState.h"
#include "states/ConfigState.h"
#include "states/DebugState.h"
#include "states/TeamsState.h"
#include "states/TimerState.h"
#include "states/LinkState.h"
#include "MacropadState.h"


class MenuState: public MacropadState
{
public:
    MenuState();

    void KeyAny(uint8_t key, bool rising, bool falling) override;

    void Key1(bool rising, bool falling) override;
    void Key2(bool rising, bool falling) override;
    void Key3(bool rising, bool falling) override;
    void Key4(bool rising, bool falling) override;
    void Key5(bool rising, bool falling) override;

    void EncoderHandler() override;
    void EncoderPress(bool rising, bool falling) override;

    void OledDraw(SH1106_SPI oled) override;

    static void UpdateMessage();

private:
    TeamsState teamsState;
    GenericState genericState;
    ConfigState configState;
    LinkState linkState;
    DebugState debugState;
    TimerState timerState;

    static char message[22];
};

