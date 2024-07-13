#include <vector>
#include <tusb.h>

#include "Macro.h"


Macro::Macro(uint8_t step_count)
    : m_StepCount(step_count)
{
    m_Steps.reserve(step_count);
}


Macro& Macro::addSleepStep(uint64_t time_ms)
{
	m_Steps.emplace_back(time_ms);
	return *this;
}

Macro& Macro::addTypingStep(const char* string)
{
	m_Steps.emplace_back(string);
	return *this;
}

Macro& Macro::addKeyPressStep(uint8_t key, uint8_t count /* = 1 */, uint8_t key2 /* = HID_KEY_NONE */, uint8_t key3 /* = HID_KEY_NONE */, uint8_t key4 /* = HID_KEY_NONE */, uint8_t key5 /* = HID_KEY_NONE */, uint8_t key6 /* = HID_KEY_NONE */)
{
	for (uint8_t i = 0; i < count; i++) {
		m_Steps.emplace_back(key, key2, key3, key4, key5, key6);
	}
	return *this;
}


void Macro::nextStep()
{
	m_CurrentStep++;
}

void Macro::restart()
{
	m_CurrentStep = 0;
}

MacroStep& Macro::getStep()
{
	return m_Steps.at(m_CurrentStep);
}

bool Macro::finished()
{
	return m_CurrentStep >= m_Steps.size();
}



MacroStep::MacroStep(const char* string)
    : step_type(TYPE_TYPE_STRING)
{
	data = reinterpret_cast<uint64_t>(string);
}

MacroStep::MacroStep(uint64_t time_ms)
    : step_type(TYPE_SLEEP), data(time_ms) { }

MacroStep::MacroStep(uint8_t key1, uint8_t key2 /* = HID_KEY_NONE */, uint8_t key3 /* = HID_KEY_NONE */, uint8_t key4 /* = HID_KEY_NONE */, uint8_t key5 /* = HID_KEY_NONE */, uint8_t key6 /* = HID_KEY_NONE */)
	: step_type(TYPE_KEY_PRESS), data(key1 | key2 << 8 | key3 << 16 | key4 << 24 | (uint64_t) key5 << 32 | (uint64_t) key6 << 40) {	}

