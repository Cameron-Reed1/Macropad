#include "Macropad.h"
#include "RippleAnim.h"
#include <string>

extern MacropadState default_state;
extern uint64_t press_start[12];

enum STEP: uint8_t {
	COLLECTION_SELECTION = 0,
	BOOK_SELECTION,
	CHAPTER_SELECTION
};

static const char* const base_url = "https://www.churchofjesuschrist.org/study/scriptures/";
static std::string url;
static const char* title;
static const char* const* friendly_names;
static bool first_page = true;
static bool last_page = false;
static int16_t offset = -1;
static uint8_t item_count;
static uint8_t selections[3] = { 0 };
static STEP current_step;

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
	50,		// gen
	40,		// ex
	27,		// lev
	36,		// num
	34,		// deut
	24,		// josh
	21,		// judg
	4,		// ruth
	31,		// 1-sam
	24,		// 2-sam
	22,		// 1-kgs
	25,		// 2-kgs
	29,		// 1-chr
	26,		// 2-chr
	10,		// ezra
	13,		// neh
	10,		// esth
	42,		// job
	150,	// ps
	31,		// prov
	12,		// eccl
	66,		// isa
	52,		// jer
	5,		// lam
	48,		// ezek
	12,		// dan
	14,		// hosea
	3,		// joel
	9,		// amos
	1,		// obad
	4,		// jonah
	7,		// micah
	3,		// nahum
	3,		// hab
	3,		// zeph
	2,		// hag
	14,		// zech
	4		// mal
};

static const uint8_t nt_chapter_count[] = {
	28,		// matt
	16,		// mark
	24,		// luke
	21,		// john
	28,		// acts
	16,		// rom
	16,		// 1-cor
	13,		// 2-cor
	6,		// gal
	6,		// eph
	4,		// philip
	4,		// col
	5,		// 1-thes
	3,		// 2-thes
	6,		// 1-tim
	4,		// 2-tim
	3,		// titus
	1,		// philem
	13,		// heb
	5,		// james
	5,		// 1-pet
	3,		// 2-pet
	5,		// 1-jn
	1,		// 2-jn
	1,		// 3-jn
	1,		// jude
	22		// rev
};

static const uint8_t bofm_chapter_count[] = {
	22,		// 1-ne
	33,		// 2-ne
	7,		// jacob
	1,		// enos
	1,		// jarom
	1,		// omni
	1,		// w-of-m
	29,		// mosiah
	63,		// alma
	16,		// hel
	30,		// 3-ne
	1,		// 4-ne
	9,		// morm
	15,		// ether
	10		// moro
};

static const uint8_t dc_chapter_count = 138;

static const uint8_t pgp_chapter_count[] = {
	8,		// moses
	5,		// abr
	1,		// js-m
	1,		// js-h
	1		// a-of-f
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

void oled_draw_link_build_state(SH1106_SPI oled)
{
	oled.gotoXY(0, 0);
	oled.print(title);

	if (first_page) {
		oled.gotoXY(0, 1);
		oled.print("Back");
	}

	for (uint8_t i = first_page ? 1 : 0; i < 12 && i + offset < item_count; i++) {
		oled.gotoXY((i % 3) * 43, (((i - (i % 3)) / 3) * 2) + 1);
		if (current_step != CHAPTER_SELECTION)
			oled.print(friendly_names[i + offset]);
		else
			oled.print(i + offset + 1);
	}
}

void reset()
{
	url = base_url;
	title = "";
	friendly_names = collection_friendly_names;
	offset = -1;
	item_count = collections_count;
	first_page = true;
	last_page = true;
	current_step = COLLECTION_SELECTION;
}

void cancel()
{
	Macropad::get_instance().load_parent_state();
}

void finish()
{
	url = base_url;

	url += collections[selections[0]];
	url += '/';
	url += books_reference[selections[0]][selections[1]];
	url += '/';
	url += std::to_string(selections[2]);

	Macropad::get_instance().load_parent_state();

	Macropad::get_instance().type(url.c_str());
}

void next_page()
{
	if (last_page)
		return;

	Macropad::get_instance().update_oled();
	offset += 12;

	first_page = false;
	if (offset + 12 >= item_count)
		last_page = true;

}

void prev_page()
{
	if (first_page)
		return;

	offset -= 12;

	last_page = false;
	if (offset == -1)
		first_page = true;

	Macropad::get_instance().update_oled();
}

void next_step(uint8_t selection)
{
	switch (current_step)
	{
		case COLLECTION_SELECTION:
			title = collection_titles[selection];
			friendly_names = book_names_reference[selection];
			offset = -1;
			item_count = book_counts[selection];
			first_page = true;
			last_page = item_count <= 11;
			current_step = BOOK_SELECTION;
			selections[0] = selection;
			if (item_count > 1)
				break;
			selection = 0;
		case BOOK_SELECTION:
			title = book_titles_reference[selections[0]][selection];
			friendly_names = nullptr;
			offset = -1;
			item_count = book_chapter_counts[selections[0]][selection];
			first_page = true;
			last_page = item_count <= 11;
			current_step = CHAPTER_SELECTION;
			selections[1] = selection;
			if (item_count > 1)
				break;
			selection = 0;
		case CHAPTER_SELECTION:
			selections[2] = selection + 1;
			finish();
			break;
	}

	Macropad::get_instance().update_oled();
}

void prev_step()
{
	switch (current_step)
	{
		case CHAPTER_SELECTION:
			title = collection_titles[selections[0]];
			friendly_names = book_names_reference[selections[0]];
			offset = (selections[1] - (selections[1] % 12)) - 1;
			item_count = book_counts[selections[0]];
			first_page = offset == -1;
			last_page = (first_page ? offset + 11 : offset + 12) >= item_count;
			current_step = BOOK_SELECTION;
			if (item_count > 1)
				break;
		case BOOK_SELECTION:
			title = "";
			friendly_names = collection_friendly_names;
			offset = -1;
			item_count = collections_count;
			first_page = true;
			last_page = true;
			current_step = COLLECTION_SELECTION;
			break;
		case COLLECTION_SELECTION:
			cancel();
			break;
	}

	Macropad::get_instance().update_oled();
}

void button_pressed(uint8_t key, bool rising, bool falling)
{
	if (rising) {
		press_start[key] = get_absolute_time();
		ripple_anim_run(key, 0, 255, 255, DIR_OUT);
	} else if (falling) {
		if (get_absolute_time() - press_start[key] > 320 * 1000)
			ripple_anim_run(key, 0, 255, 0, DIR_IN);

		if (first_page && key == 0) {
			prev_step();
		} else {
			uint8_t selection = offset + key;
			if (selection < item_count) {
				next_step(selection);
			}
		}
	}
}

void on_encoder_tick(int last_position, int new_position)
{
	if (new_position - last_position > 0) {
		next_page();
	} else if (new_position - last_position < 0) {
		prev_page();
	}
}

void build_link_macro(bool rising, bool falling)
{
	(void) rising;

	static MacropadState link_build_state = MacropadState(&default_state, button_pressed);

	if (falling)
	{
		link_build_state.set_encoder_callback(on_encoder_tick);
		link_build_state.set_oled_draw_function(oled_draw_link_build_state);
		link_build_state.set_oled_automatic_updates(false);

		reset();

		Macropad::get_instance().set_macropad_state(&link_build_state);
	}
}
