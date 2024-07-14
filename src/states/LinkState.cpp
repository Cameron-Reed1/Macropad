#include "states/LinkState.h"
#include "states/common.h"
#include "MacropadState.h"
#include "Macropad.h"

#include <string.h>


LinkState::LinkState(MacropadState* parent)
    : MacropadState(parent) { }



static const char* const base_url = "https://www.churchofjesuschrist.org/study/scriptures/";

void LinkState::Activate()
{
    Reset();

    Macropad::get_instance().set_macropad_state(this);
}

void LinkState::KeyAny(uint8_t key, bool rising, bool falling)
{
    ripple(key, rising, falling);

    if (falling) {
        if (m_FirstPage && key == 0) {
            PrevStep();
        } else {
            uint8_t selection = m_Offset + key;
            if (selection < m_ItemCount) {
                NextStep(selection);
            }
        }
    }
}

void LinkState::EncoderHandler()
{
    if (m_EncoderPosition - m_EncoderLastPosition > 0) {
        NextPage();
    } else if (m_EncoderPosition - m_EncoderLastPosition < 0) {
        PrevPage();
    }
}

void LinkState::OledDraw(SH1106_SPI oled)
{
    oled.gotoXY(0, 0);
    oled.print(m_Title);

    if (m_FirstPage) {
        oled.gotoXY(0, 1);
        oled.print("Back");
    }

    for (uint8_t i = m_FirstPage ? 1 : 0; i < 12 && i + m_Offset < m_ItemCount; i++) {
        oled.gotoXY((i % 3) * 43, (((i - (i % 3)) / 3) * 2) + 1);
        if (m_CurrentStep != CHAPTER_SELECTION) {
            oled.print(m_FriendlyNames[i + m_Offset]);
        } else {
            oled.print(i + m_Offset + 1);
        }
    }
}



static const char* const collections[] = {
    "ot",
    "nt",
    "bofm",
    "dc-testament",
    "pgp"
};

static const char* const ot_books[] = {
    "gen",
    "ex",
    "lev",
    "num",
    "deut",
    "josh",
    "judg",
    "ruth",
    "1-sam",
    "2-sam",
    "1-kgs",
    "2-kgs",
    "1-chr",
    "2-chr",
    "ezra",
    "neh",
    "esth",
    "job",
    "ps",
    "prov",
    "eccl",
    "isa",
    "jer",
    "lam",
    "ezek",
    "dan",
    "hosea",
    "joel",
    "amos",
    "obad",
    "jonah",
    "micah",
    "nahum",
    "hab",
    "zeph",
    "hag",
    "zech",
    "mal"
};

static const char* const nt_books[] = {
    "matt",
    "mark",
    "luke",
    "john",
    "acts",
    "rom",
    "1-cor",
    "2-cor",
    "gal",
    "eph",
    "philip",
    "col",
    "1-thes",
    "2-thes",
    "1-tim",
    "2-tim",
    "titus",
    "philem",
    "heb",
    "james",
    "1-pet",
    "2-pet",
    "1-jn",
    "2-jn",
    "3-jn",
    "jude",
    "rev"
};

static const char* const bofm_books[] = {
    "1-ne",
    "2-ne",
    "jacob",
    "enos",
    "jarom",
    "omni",
    "w-of-m",
    "mosiah",
    "alma",
    "hel",
    "3-ne",
    "4-ne",
    "morm",
    "ether",
    "moro"
};

static const char* const dc_book = "dc";

static const char* const pgp_books[] = {
    "moses",
    "abr",
    "js-m",
    "js-h",
    "a-of-f"
};

static const char* const collection_friendly_names[] = {
    "Old",
    "New",
    "B of M",
    "D&C",
    "P of GP"
};

static const char* const ot_books_friendly_names[] = {
    "Genesis",
    "Exodus",
    "Levit",
    "Numbers",
    "Deut",
    "Joshua",
    "Judges",
    "Ruth",
    "1 Sam",
    "2 Sam",
    "1 Kings",
    "2 Kings",
    "1 Chr",
    "2 Chr",
    "Ezra",
    "Nehem",
    "Esther",
    "Job",
    "Psalms",
    "Prov",
    "Eccl",
    "Isaiah",
    "Jerem",
    "Lamen",
    "Ezekiel",
    "Daniel",
    "Hosea",
    "Joel",
    "Amos",
    "Obad",
    "Jonah",
    "Micah",
    "Nahum",
    "Habak",
    "Zephan",
    "Haggai",
    "Zechar",
    "Malachi"
};

static const char* const nt_books_friendly_names[] = {
    "Matt",
    "Mark",
    "Luke",
    "John",
    "Acts",
    "Romans",
    "1 Cor",
    "2 Cor",
    "Galat",
    "Eph",
    "Philip",
    "Colos",
    "1 Thes",
    "2 Thes",
    "1 Tim",
    "2 Tim",
    "Titus",
    "Philem",
    "Hebrews",
    "James",
    "1 Peter",
    "2 Peter",
    "1 John",
    "2 John",
    "3 John",
    "Jude",
    "Revel"
};

static const char* const bofm_books_friendly_names[] = {
    "1 Nephi",
    "2 Nephi",
    "Jacob",
    "Enos",
    "Jarom",
    "Omni",
    "W of M",
    "Mosiah",
    "Alma",
    "Helaman",
    "3 Nephi",
    "4 Nephi",
    "Mormon",
    "Ether",
    "Moroni"
};

static const char* const pgp_books_friendly_names[] = {
    "Moses",
    "Abraham",
    "JS M",
    "JS H",
    "A of F"
};

static const char* const collection_titles[] = {
    "Old Testament",
    "New Testament",
    "Book of Mormon",
    "Doctrince & Covenents",
    "Pearl of Great Price"
};

static const char* const ot_books_titles[] = {
    "Genesis",
    "Exodus",
    "Leviticus",
    "Numbers",
    "Deuteronomy",
    "Joshua",
    "Judges",
    "Ruth",
    "1 Samuel",
    "2 Samuel",
    "1 Kings",
    "2 Kings",
    "1 Chronicles",
    "2 Chronicles",
    "Ezra",
    "Nehemiah",
    "Esther",
    "Job",
    "Psalms",
    "Proverbs",
    "Ecclesiastes",
    "Isaiah",
    "Jeremiah",
    "Lamentations",
    "Ezekiel",
    "Daniel",
    "Hosea",
    "Joel",
    "Amos",
    "Obadiah",
    "Jonah",
    "Micah",
    "Nahum",
    "Habakkuk",
    "Zephaniah",
    "Haggai",
    "Zechariah",
    "Malachi"
};

static const char* const nt_books_titles[] = {
    "Matt",
    "Mark",
    "Luke",
    "John",
    "Acts",
    "Romans",
    "1 Corinthians",
    "2 Corinthians",
    "Galatians",
    "Ephesians",
    "Philipians",
    "Colossians",
    "1 Thessalonians",
    "2 Thessalonians",
    "1 Timothy",
    "2 Timothy",
    "Titus",
    "Philemon",
    "Hebrews",
    "James",
    "1 Peter",
    "2 Peter",
    "1 John",
    "2 John",
    "3 John",
    "Jude",
    "Revelations"
};

static const char* const pgp_books_titles[] = {
    "Moses",
    "Abraham",
    "JS Matthew",
    "JS History",
    "Articles of Faith"
};

static const uint8_t ot_chapter_count[] = {
    50,        // gen
    40,        // ex
    27,        // lev
    36,        // num
    34,        // deut
    24,        // josh
    21,        // judg
    4,        // ruth
    31,        // 1-sam
    24,        // 2-sam
    22,        // 1-kgs
    25,        // 2-kgs
    29,        // 1-chr
    26,        // 2-chr
    10,        // ezra
    13,        // neh
    10,        // esth
    42,        // job
    150,    // ps
    31,        // prov
    12,        // eccl
    66,        // isa
    52,        // jer
    5,        // lam
    48,        // ezek
    12,        // dan
    14,        // hosea
    3,        // joel
    9,        // amos
    1,        // obad
    4,        // jonah
    7,        // micah
    3,        // nahum
    3,        // hab
    3,        // zeph
    2,        // hag
    14,        // zech
    4        // mal
};

static const uint8_t nt_chapter_count[] = {
    28,        // matt
    16,        // mark
    24,        // luke
    21,        // john
    28,        // acts
    16,        // rom
    16,        // 1-cor
    13,        // 2-cor
    6,        // gal
    6,        // eph
    4,        // philip
    4,        // col
    5,        // 1-thes
    3,        // 2-thes
    6,        // 1-tim
    4,        // 2-tim
    3,        // titus
    1,        // philem
    13,        // heb
    5,        // james
    5,        // 1-pet
    3,        // 2-pet
    5,        // 1-jn
    1,        // 2-jn
    1,        // 3-jn
    1,        // jude
    22        // rev
};

static const uint8_t bofm_chapter_count[] = {
    22,        // 1-ne
    33,        // 2-ne
    7,        // jacob
    1,        // enos
    1,        // jarom
    1,        // omni
    1,        // w-of-m
    29,        // mosiah
    63,        // alma
    16,        // hel
    30,        // 3-ne
    1,        // 4-ne
    9,        // morm
    15,        // ether
    10        // moro
};

static const uint8_t dc_chapter_count = 138;

static const uint8_t pgp_chapter_count[] = {
    8,        // moses
    5,        // abr
    1,        // js-m
    1,        // js-h
    1        // a-of-f
};

static const char* const * const books_reference[] = {
    ot_books,
    nt_books,
    bofm_books,
    &dc_book,
    pgp_books
};

static const char* const * const book_names_reference[] = {
    ot_books_friendly_names,
    nt_books_friendly_names,
    bofm_books_friendly_names,
    &collection_friendly_names[3],
    pgp_books_friendly_names
};

static const char* const * const book_titles_reference[] = {
    ot_books_titles,
    nt_books_titles,
    bofm_books_friendly_names,
    &collection_titles[3],
    pgp_books_titles
};

static const uint8_t collections_count = sizeof(collections) / sizeof(collections[0]);

static const uint8_t book_counts[] = {
    sizeof(ot_books) / sizeof(ot_books[0]),
    sizeof(nt_books) / sizeof(nt_books[0]),
    sizeof(bofm_books) / sizeof(bofm_books[0]),
    1,
    sizeof(pgp_books) / sizeof(pgp_books[0])
};

static const uint8_t* book_chapter_counts[] = {
    ot_chapter_count,
    nt_chapter_count,
    bofm_chapter_count,
    &dc_chapter_count,
    pgp_chapter_count
};



void LinkState::Reset()
{
    m_URL = base_url;
    m_Title = "";
    m_FriendlyNames = collection_friendly_names;
    m_Offset = -1;
    m_ItemCount = collections_count;
    m_FirstPage = true;
    m_LastPage = true;
    m_CurrentStep = COLLECTION_SELECTION;
}

void LinkState::Cancel()
{
    Macropad::get_instance().load_parent_state();
}

void LinkState::Finish()
{
    m_URL = base_url;

    m_URL += collections[m_Selections[0]];
    m_URL += '/';
    m_URL += books_reference[m_Selections[0]][m_Selections[1]];
    m_URL += '/';
    m_URL += std::to_string(m_Selections[2]);

    Macropad::get_instance().load_parent_state();

    Macropad::get_instance().type(m_URL.c_str());
}

void LinkState::NextPage()
{
    if (m_LastPage) {
        return;
    }

    Macropad::get_instance().update_oled();
    m_Offset += 12;

    m_FirstPage = false;
    if (m_Offset + 12 >= m_ItemCount) {
        m_LastPage = true;
    }
}

void LinkState::PrevPage()
{
    if (m_FirstPage) {
        return;
    }

    m_Offset -= 12;

    m_LastPage = false;
    if (m_Offset == -1) {
        m_FirstPage = true;
    }

    Macropad::get_instance().update_oled();
}

void LinkState::NextStep(uint8_t selection)
{
    switch (m_CurrentStep)
    {
        case COLLECTION_SELECTION:
            m_Title = collection_titles[selection];
            m_FriendlyNames = book_names_reference[selection];
            m_Offset = -1;
            m_ItemCount = book_counts[selection];
            m_FirstPage = true;
            m_LastPage = m_ItemCount <= 11;
            m_CurrentStep = BOOK_SELECTION;
            m_Selections[0] = selection;
            if (m_ItemCount > 1)
                break;
            selection = 0;
        case BOOK_SELECTION:
            m_Title = book_titles_reference[m_Selections[0]][selection];
            m_FriendlyNames = nullptr;
            m_Offset = -1;
            m_ItemCount = book_chapter_counts[m_Selections[0]][selection];
            m_FirstPage = true;
            m_LastPage = m_ItemCount <= 11;
            m_CurrentStep = CHAPTER_SELECTION;
            m_Selections[1] = selection;
            if (m_ItemCount > 1)
                break;
            selection = 0;
        case CHAPTER_SELECTION:
            m_Selections[2] = selection + 1;
            Finish();
            break;
    }

    Macropad::get_instance().update_oled();
}

void LinkState::PrevStep()
{
    switch (m_CurrentStep) {
        case CHAPTER_SELECTION:
            m_Title = collection_titles[m_Selections[0]];
            m_FriendlyNames = book_names_reference[m_Selections[0]];
            m_Offset = (m_Selections[1] - (m_Selections[1] % 12)) - 1;
            m_ItemCount = book_counts[m_Selections[0]];
            m_FirstPage = m_Offset == -1;
            m_LastPage = (m_FirstPage ? m_Offset + 11 : m_Offset + 12) >= m_ItemCount;
            m_CurrentStep = BOOK_SELECTION;
            if (m_ItemCount > 1)
                break;
        case BOOK_SELECTION:
            m_Title = "";
            m_FriendlyNames = collection_friendly_names;
            m_Offset = -1;
            m_ItemCount = collections_count;
            m_FirstPage = true;
            m_LastPage = true;
            m_CurrentStep = COLLECTION_SELECTION;
            break;
        case COLLECTION_SELECTION:
            Cancel();
            break;
    }

    Macropad::get_instance().update_oled();
}

