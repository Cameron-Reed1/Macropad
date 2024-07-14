#pragma once

#include <stdint.h>
#include <vector>
#include <tusb.h>


enum MACRO_STEP_TYPE: uint8_t {
    TYPE_KEY_PRESS = 0,
    TYPE_TYPE_STRING,
    TYPE_SLEEP
};


class MacroStep
{
public:
    MacroStep() = delete;
    MacroStep(uint64_t time_ms);
    MacroStep(uint8_t key1, uint8_t key2 = HID_KEY_NONE, uint8_t key3 = HID_KEY_NONE, uint8_t key4 = HID_KEY_NONE, uint8_t key5 = HID_KEY_NONE, uint8_t key6 = HID_KEY_NONE);
    MacroStep(const char* string);

public:
    MACRO_STEP_TYPE step_type;
    uint64_t data;
};


class Macro
{
public:
    Macro(uint8_t step_count);

    Macro& addKeyPressStep(uint8_t key, uint8_t count = 1, uint8_t key2 = HID_KEY_NONE, uint8_t key3 = HID_KEY_NONE, uint8_t key4 = HID_KEY_NONE, uint8_t key5 = HID_KEY_NONE, uint8_t key6 = HID_KEY_NONE);
    Macro& addTypingStep(const char* string);
    Macro& addSleepStep(uint64_t time_ms);

    MacroStep& getStep();
    void nextStep();
    void restart();
    bool finished();

private:
    std::vector<MacroStep> m_Steps;
    uint8_t m_CurrentStep = 0;
    uint8_t m_StepCount;
};
