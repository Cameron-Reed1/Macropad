#include <boards/adafruit_macropad_rp2040.h>
#include <hardware/flash.h>
#include <hardware/sync.h>
#include <cstring>

#include "Config.h"
#include "hardware/regs/addressmap.h"


static_assert(PICO_FLASH_SIZE_BYTES % FLASH_SECTOR_SIZE == 0, "Flash does not fit a whole number of sectors");
static_assert(FLASH_SECTOR_SIZE % FLASH_PAGE_SIZE == 0, "Flash sectors do not fit a whole number of pages");
static_assert(sizeof(CFG) <= FLASH_PAGE_SIZE, "CFG struct will not fit in a flash page");


static uint32_t CONFIG_SLOT_SECTOR_OFFSETS[NUM_SLOTS] = {
    PICO_FLASH_SIZE_BYTES - (FLASH_SECTOR_SIZE * 1), PICO_FLASH_SIZE_BYTES - (FLASH_SECTOR_SIZE * 2),
    PICO_FLASH_SIZE_BYTES - (FLASH_SECTOR_SIZE * 3), PICO_FLASH_SIZE_BYTES - (FLASH_SECTOR_SIZE * 4),
    PICO_FLASH_SIZE_BYTES - (FLASH_SECTOR_SIZE * 5), PICO_FLASH_SIZE_BYTES - (FLASH_SECTOR_SIZE * 6),
    PICO_FLASH_SIZE_BYTES - (FLASH_SECTOR_SIZE * 7), PICO_FLASH_SIZE_BYTES - (FLASH_SECTOR_SIZE * 8),
    PICO_FLASH_SIZE_BYTES - (FLASH_SECTOR_SIZE * 9), PICO_FLASH_SIZE_BYTES - (FLASH_SECTOR_SIZE * 10),
};
static uint32_t CONFIG_SECTOR_OFFSET = CONFIG_SLOT_SECTOR_OFFSETS[0];
static uint8_t* CONFIG_SECTOR_ADDR = reinterpret_cast<uint8_t*>(XIP_NOCACHE_NOALLOC_BASE + CONFIG_SECTOR_OFFSET);
static uint32_t SLOT_INDEX_SECTOR_OFFSET = PICO_FLASH_SIZE_BYTES - (FLASH_SECTOR_SIZE * (NUM_SLOTS + 1));
static uint8_t* SLOT_INDEX_SECTOR_ADDR = reinterpret_cast<uint8_t*>(XIP_NOCACHE_NOALLOC_BASE + SLOT_INDEX_SECTOR_OFFSET);

static uint8_t config_cache[FLASH_PAGE_SIZE] = { };
static uint8_t index_cache[FLASH_PAGE_SIZE] = { };
static uint8_t index_page = 0;
static uint8_t index_index = 0;
static uint8_t slot_index = NUM_SLOTS;


CFG defaults = {
    .brightness = 80,

    .colorOutR = 0x00,
    .colorOutG = 0xFF,
    .colorOutB = 0xFF,
    
    .colorInR = 0x00,
    .colorInG = 0xFF,
    .colorInB = 0x00,
};

namespace Config {

void _getSavedSlot();
uint8_t _getWritePage();
void _switchSlot(uint8_t slot);


CFG* read()
{
    if (slot_index >= NUM_SLOTS) {
        _getSavedSlot();
    }

    uint8_t page = _getWritePage();
    if (page == 0) {
        memcpy(config_cache, &defaults, sizeof(CFG));
        return get();
    }
    page--;

    memcpy(config_cache, CONFIG_SECTOR_ADDR + (page * FLASH_PAGE_SIZE), FLASH_PAGE_SIZE);
    return get();
}

CFG* get()
{
    return reinterpret_cast<CFG*>(config_cache);
}

uint8_t getSlot()
{
    return slot_index;
}

CFG* savedValues()
{
    uint8_t page = _getWritePage();
    if (page != 0) {
        page--;
    }

    return reinterpret_cast<CFG*>(CONFIG_SECTOR_ADDR + (page * FLASH_PAGE_SIZE));
}

void switchSlot(uint8_t slot)
{
    if (slot >= NUM_SLOTS || slot == slot_index) {
        return;
    }

    _switchSlot(slot);
    read();
}

void erase()
{
    uint32_t ints = save_and_disable_interrupts();
    flash_range_erase(CONFIG_SECTOR_OFFSET, FLASH_SECTOR_SIZE);
    restore_interrupts(ints);
}

void eraseAll()
{
    uint32_t ints = save_and_disable_interrupts();
    flash_range_erase(SLOT_INDEX_SECTOR_OFFSET, FLASH_SECTOR_SIZE * (NUM_SLOTS + 1));
    restore_interrupts(ints);
}

void flush()
{
    uint8_t page = _getWritePage();
    uint32_t ints = save_and_disable_interrupts();

    if (page >= FLASH_SECTOR_SIZE / FLASH_PAGE_SIZE) {
        flash_range_erase(CONFIG_SECTOR_OFFSET, FLASH_SECTOR_SIZE);
        page = 0;
    }

    flash_range_program(CONFIG_SECTOR_OFFSET + (page * FLASH_PAGE_SIZE), config_cache, FLASH_PAGE_SIZE);
    restore_interrupts(ints);
}

void _getSavedSlot()
{
    for (index_page = 0; index_page < FLASH_SECTOR_SIZE / FLASH_PAGE_SIZE; index_page++) {
        memcpy(index_cache, SLOT_INDEX_SECTOR_ADDR + (index_page * FLASH_PAGE_SIZE), FLASH_PAGE_SIZE);
        if (index_cache[FLASH_PAGE_SIZE - 1] == 0xFF) {
            break;
        }
    }

    if (index_page >= FLASH_SECTOR_SIZE / FLASH_PAGE_SIZE) {
        index_page = FLASH_SECTOR_SIZE / FLASH_PAGE_SIZE - 1;
    }

    for (index_index = 0; true; index_index++) {
        if (index_cache[index_index] == 0xFF) {
            break;
        }
    }

    if (index_index == 0) {
        if (index_page == 0) {
            slot_index = 0;
        } else {
            index_page--;
            index_index = 255;
            memcpy(index_cache, SLOT_INDEX_SECTOR_ADDR + (index_page * FLASH_PAGE_SIZE), FLASH_PAGE_SIZE);
            slot_index = index_cache[index_index];
        }
    } else {
        index_index--;
        slot_index = index_cache[index_index];
    }
    _switchSlot(slot_index);
}

uint8_t _getWritePage()
{
    uint8_t page = 0;

    for (; page * FLASH_PAGE_SIZE < FLASH_SECTOR_SIZE; page++) {
        uint32_t start = *(uint32_t*) (CONFIG_SECTOR_ADDR + (page * FLASH_PAGE_SIZE));
        if (start == 0xFFFFFFFF) {
            break;
        }
    }

    return page;
}

void _switchSlot(uint8_t slot)
{
    slot_index = slot;
    CONFIG_SECTOR_OFFSET = CONFIG_SLOT_SECTOR_OFFSETS[slot_index];
    CONFIG_SECTOR_ADDR = reinterpret_cast<uint8_t*>(XIP_NOCACHE_NOALLOC_BASE + CONFIG_SECTOR_OFFSET);

    uint8_t saved_index = index_cache[index_index];

    if (slot_index == saved_index) {
        return;
    }

    if (saved_index != 0xFF) {
        if (index_index == FLASH_PAGE_SIZE - 1) {
            index_index = 0;
            index_page++;
            if (index_page >= FLASH_SECTOR_SIZE / FLASH_PAGE_SIZE) {
                index_page = 0;
                uint32_t ints = save_and_disable_interrupts();
                flash_range_erase(SLOT_INDEX_SECTOR_OFFSET, FLASH_SECTOR_SIZE);
                restore_interrupts(ints);
            }
            memcpy(index_cache, SLOT_INDEX_SECTOR_ADDR + (index_page * FLASH_PAGE_SIZE), FLASH_PAGE_SIZE);
        } else {
            index_index++;
        }
    }

    index_cache[index_index] = slot_index;

    uint32_t ints = save_and_disable_interrupts();
    flash_range_program(SLOT_INDEX_SECTOR_OFFSET + (index_page * FLASH_PAGE_SIZE), index_cache, FLASH_PAGE_SIZE);
    restore_interrupts(ints);
}

}

