#include <boards/adafruit_macropad_rp2040.h>
#include <hardware/flash.h>
#include <hardware/sync.h>
#include <cstring>

#include "Config.h"


static_assert(PICO_FLASH_SIZE_BYTES % FLASH_SECTOR_SIZE == 0, "Flash does not fit a whole number of sectors");
static_assert(FLASH_SECTOR_SIZE % FLASH_PAGE_SIZE == 0, "Flash sectors do not fit a whole number of pages");
static_assert(sizeof(CFG) <= FLASH_PAGE_SIZE, "CFG struct will not fit in a flash page");


static uint32_t CONFIG_SECTOR_OFFSET = PICO_FLASH_SIZE_BYTES - (FLASH_SECTOR_SIZE * 10);
static uint8_t* CONFIG_SECTOR_ADDR = reinterpret_cast<uint8_t*>(XIP_BASE + CONFIG_SECTOR_OFFSET);
uint8_t config_cache[FLASH_PAGE_SIZE] = { };


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

uint8_t getWritePage()
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

CFG* read()
{
    uint8_t page = getWritePage();
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
    return reinterpret_cast<CFG*>(&config_cache[0]);
}

void erase()
{
    uint32_t ints = save_and_disable_interrupts();
    flash_range_erase(CONFIG_SECTOR_OFFSET, FLASH_SECTOR_SIZE);
    restore_interrupts(ints);
}

CFG* savedValues()
{
    uint8_t page = getWritePage();
    if (page != 0) {
        page--;
    }

    return reinterpret_cast<CFG*>(CONFIG_SECTOR_ADDR + (page * FLASH_PAGE_SIZE));
}

void flush()
{
    uint8_t page = getWritePage();
    uint32_t ints = save_and_disable_interrupts();

    if (page >= FLASH_SECTOR_SIZE / FLASH_PAGE_SIZE) {
        flash_range_erase(CONFIG_SECTOR_OFFSET, FLASH_SECTOR_SIZE);
        page = 0;
    }

    flash_range_program(CONFIG_SECTOR_OFFSET + (page * FLASH_PAGE_SIZE), &config_cache[0], FLASH_PAGE_SIZE);
    restore_interrupts(ints);
}

}

