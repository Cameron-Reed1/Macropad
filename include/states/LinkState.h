#pragma once

#include <string>
#include "MacropadState.h"


enum STEP: uint8_t {
	COLLECTION_SELECTION = 0,
	BOOK_SELECTION,
	CHAPTER_SELECTION
};


class LinkState: public MacropadState
{
public:
    LinkState() = delete;
    LinkState(MacropadState* parent);

    void Activate() override;

    void KeyAny(uint8_t key, bool rising, bool falling) override;

    void EncoderHandler() override;
    void OledDraw(SH1106_SPI oled) override;

private:
    void Reset();
    void Cancel();
    void Finish();
    void NextPage();
    void PrevPage();
    void NextStep(uint8_t selection);
    void PrevStep();

private:
    std::string m_URL;
    const char* m_Title;
    const char* const* m_FriendlyNames;
    bool m_FirstPage = true;
    bool m_LastPage = false;
    int16_t m_Offset = -1;
    uint8_t m_ItemCount;
    uint8_t m_Selections[3] = { 0 };
    STEP m_CurrentStep;
};

