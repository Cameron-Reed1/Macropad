#include "Macro.h"

Macro::Macro(uint8_t step_count): m_step_count(step_count) {
	m_steps.reserve(step_count);
}

Macro &Macro::addSleepStep(uint64_t time_ms)
{
	m_steps.emplace_back(time_ms);
	return *this;
}

Macro &Macro::addTypingStep(const char* string)
{
	m_steps.emplace_back(string);
	return *this;
}

Macro &Macro::addKeyPressStep(uint8_t key, uint8_t count /* = 1 */, uint8_t key2 /* = HID_KEY_NONE */, uint8_t key3 /* = HID_KEY_NONE */, uint8_t key4 /* = HID_KEY_NONE */, uint8_t key5 /* = HID_KEY_NONE */, uint8_t key6 /* = HID_KEY_NONE */)
{
	for (uint8_t i = 0; i < count; i++)
	{
		m_steps.emplace_back(key, key2, key3, key4, key5, key6);
	}
	return *this;
}

void Macro::nextStep()
{
	m_current_step++;
}

void Macro::restart()
{
	m_current_step = 0;
}

MacroStep& Macro::getStep()
{
	return m_steps.at(m_current_step);
}

bool Macro::finished()
{
	return m_current_step >= m_steps.size();
}

MacroStep::MacroStep(const char* string) : m_step_type(TYPE_TYPE_STRING)
{
	m_data = reinterpret_cast<uint64_t>(string);
}

MacroStep::MacroStep(uint64_t time_ms) : m_step_type(TYPE_SLEEP), m_data(time_ms) {}

MacroStep::MacroStep(uint8_t key1, uint8_t key2 /* = HID_KEY_NONE */, uint8_t key3 /* = HID_KEY_NONE */, uint8_t key4 /* = HID_KEY_NONE */, uint8_t key5 /* = HID_KEY_NONE */, uint8_t key6 /* = HID_KEY_NONE */)
	: m_step_type(TYPE_KEY_PRESS), m_data(key1 | key2 << 8 | key3 << 16 | key4 << 24 | (uint64_t) key5 << 32 | (uint64_t) key6 << 40) {	}
