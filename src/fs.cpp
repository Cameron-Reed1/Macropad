#include <hardware/regs/addressmap.h>
#include <hardware/sync.h>
#include <lfs.h>
#include <hardware/flash.h>
#include <hardware/gpio.h>
#include <stdio.h>
#include <string.h>

#include "Macropad.h"
#include "fs.h"



static lfs_t lfs;

int lfs_read(const lfs_config* cfg, lfs_block_t block, lfs_off_t off, void* buffer, lfs_size_t size);
int lfs_prog(const lfs_config* cfg, lfs_block_t block, lfs_off_t off, const void* buffer, lfs_size_t size);
int lfs_erase(const lfs_config* cfg, lfs_block_t block);
int lfs_sync(const lfs_config* cfg);

lfs_config lfs_cfg = {
    .read = lfs_read,
    .prog = lfs_prog,
    .erase = lfs_erase,
    .sync = lfs_sync,

    .read_size = FLASH_PAGE_SIZE,
    .prog_size = FLASH_PAGE_SIZE,
    .block_size = FLASH_SECTOR_SIZE,
    .block_count = fs::SIZE / FLASH_SECTOR_SIZE,
    .block_cycles = 500,
    .cache_size = FLASH_PAGE_SIZE,
    .lookahead_size = 16,
};


namespace fs {


const uint32_t FLASH_OFFSET = PICO_FLASH_SIZE_BYTES - (2 * 1024 * 1024);
const uint8_t* FLASH_START = reinterpret_cast<uint8_t*>(XIP_BASE + FLASH_OFFSET);
const uint32_t SIZE = 1024 * 1024;


void init()
{
    // If the lfs struct has already been initialized, this function was called already
    if (lfs.cfg == &lfs_cfg) {
        return;
    }

    int err = lfs_mount(&lfs, &lfs_cfg);

    if (err) {
        err = lfs_format(&lfs, &lfs_cfg);
        if (err) return;

        err = lfs_mount(&lfs, &lfs_cfg);
        if (err) return;
    }

    // Enable USB MSC if we were able to mount the filesystem
    Macropad::get_instance().EnableMSC = true;
}


File::File(const char* const path, int flags)
{
    open_err = lfs_file_open(&lfs, &file, path, flags);
    closed = open_err < 0;
}

lfs_size_t File::read(void* buffer, lfs_size_t size)
{
    return lfs_file_read(&lfs, &file, buffer, size);
}

int File::close()
{
    int err = lfs_file_close(&lfs, &file);
    closed = true;
    return err;
}

File::~File()
{
    close();
}


Dir::Dir(const char* const path)
{
    open_err = lfs_dir_open(&lfs, &dir, path);
}

int Dir::close()
{
    return lfs_dir_close(&lfs, &dir);
}


Dir::iterator Dir::begin()
{
    return ++iterator(this);
}

Dir::iterator Dir::end()
{
    return iterator();
}

Dir::iterator::iterator()
    : m_dir(nullptr), m_end(true) {}

Dir::iterator::iterator(Dir* dir)
    : m_dir(dir), m_end(false) {}

Dir::iterator::value_type Dir::iterator::operator*() const
{
    if (m_end) {
        return nullptr;
    }
    return &m_current;
}

Dir::iterator& Dir::iterator::operator++()
{
    if (m_end) {
        return *this;
    }

    int ret = lfs_dir_read(&lfs, &m_dir->dir, &m_current);
    if (ret <= 0) {
        m_end = true;
    }
    return *this;
}

bool operator==(const Dir::iterator& lhs, const Dir::iterator& rhs)
{
    if (lhs.m_end && rhs.m_end) {
        return true;
    }

    if (lhs.m_end || rhs.m_end) {
        return false;
    }

    return strcmp(lhs.m_current.name, rhs.m_current.name) == 0;
}

bool operator!=(const Dir::iterator& lhs, const Dir::iterator& rhs)
{
    return !(lhs == rhs);
}

} // namespace fs



int lfs_read(const lfs_config* cfg, lfs_block_t block, lfs_off_t off, void* buffer, lfs_size_t size)
{
    memcpy(buffer, fs::FLASH_START + (cfg->block_size * block) + off, size);

    return 0;
}

int lfs_prog(const lfs_config* cfg, lfs_block_t block, lfs_off_t off, const void* buffer, lfs_size_t size)
{
    uint32_t ints = save_and_disable_interrupts();
    flash_range_program(fs::FLASH_OFFSET + (cfg->block_size * block) + off, (uint8_t*) buffer, size);
    restore_interrupts_from_disabled(ints);

    return 0;
}

int lfs_erase(const lfs_config* cfg, lfs_block_t block)
{
    uint32_t ints = save_and_disable_interrupts();
    flash_range_erase(fs::FLASH_OFFSET + (cfg->block_size * block), FLASH_SECTOR_SIZE);
    restore_interrupts_from_disabled(ints);

    return 0;
}

int lfs_sync(const lfs_config* cfg)
{
    (void) cfg;
    flash_flush_cache();

    return 0;
}
