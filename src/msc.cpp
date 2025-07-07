#include <hardware/flash.h>
#include <hardware/regs/addressmap.h>
#include <hardware/sync.h>
#include <pico.h>
#include <boards/adafruit_macropad_rp2040.h>
#include "class/msc/msc_device.h"
#include <cstdint>
#include <tusb.h>

#include "Macropad.h"
#include "states/MenuState.h"
#include "fs.h"


extern "C" {


const uint32_t DISK_BLOCK_SIZE = FLASH_SECTOR_SIZE;
const uint32_t DISK_BLOCK_NUM  = 1 + (fs::SIZE / DISK_BLOCK_SIZE);


static uint8_t cached_sector[FLASH_SECTOR_SIZE];
static int16_t cached_sector_id = -1;


static const uint8_t MBR[512] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    0x00, 0x00, 0x10, 0x00, 0xC3, 0x00, 0x10, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, // Partition entry #1
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Partition entry #2 (empty)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Partition entry #3 (empty)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Partition entry #4 (empty)
    0x55, 0xAA, // Boot signature
};


void tud_msc_inquiry_cb(uint8_t lun, uint8_t vendor_id[8], uint8_t product_id[16], uint8_t product_rev[4])
{
    (void) lun;

    const char vid[] = "TinyUSB";
    const char pid[] = "Mass Storage";
    const char rev[] = "1.0";

    memcpy(vendor_id  , vid, sizeof(vid));
    memcpy(product_id , pid, sizeof(pid));
    memcpy(product_rev, rev, sizeof(rev));
}

// Invoked when received Test Unit Ready command.
// return true allowing host to read/write this LUN e.g SD card inserted
bool tud_msc_test_unit_ready_cb(uint8_t lun)
{
    (void) lun;

    // RAM disk is ready until ejected
    if (!Macropad::get_instance().EnableMSC) {
        // Additional Sense 3A-00 is NOT_FOUND
        tud_msc_set_sense(lun, SCSI_SENSE_NOT_READY, 0x3a, 0x00);
        return false;
    }

    return true;
}

// Invoked when received SCSI_CMD_READ_CAPACITY_10 and SCSI_CMD_READ_FORMAT_CAPACITY to determine the disk size
// Application update block count and block size
void tud_msc_capacity_cb(uint8_t lun, uint32_t* block_count, uint16_t* block_size)
{
    (void) lun;

    *block_count = DISK_BLOCK_NUM;
    *block_size  = DISK_BLOCK_SIZE;
}

// Invoked when received Start Stop Unit command
// - Start = 0 : stopped power mode, if load_eject = 1 : unload disk storage
// - Start = 1 : active mode, if load_eject = 1 : load disk storage
bool tud_msc_start_stop_cb(uint8_t lun, uint8_t power_condition, bool start, bool load_eject)
{
    (void) lun;
    (void) power_condition;

    if (load_eject) {
        if (start) {
            // load disk storage
        } else {
            // unload disk storage
            if (cached_sector_id >= 0) {
                uint32_t ints = save_and_disable_interrupts();
                flash_range_erase(fs::FLASH_OFFSET + (FLASH_SECTOR_SIZE * cached_sector_id), FLASH_SECTOR_SIZE);
                flash_range_program(fs::FLASH_OFFSET + (FLASH_SECTOR_SIZE * cached_sector_id), cached_sector, FLASH_SECTOR_SIZE);
                restore_interrupts(ints);
            }
            MenuState::UpdateMessage();
        }
    }

    return true;
}

// Callback invoked when received READ10 command.
// Copy disk's data to buffer (up to bufsize) and return number of copied bytes.
int32_t tud_msc_read10_cb(uint8_t lun, uint32_t lba, uint32_t offset, void* buffer, uint32_t bufsize)
{
    (void) lun;

    // out of ramdisk
    if (lba >= DISK_BLOCK_NUM) {
        return -1;
    }

    uint32_t read_size = bufsize;
    if (lba == 0) {
        const uint32_t mbr_offset = offset % sizeof(MBR);
        const uint8_t* addr = MBR + mbr_offset;
        if (mbr_offset + bufsize > sizeof(MBR)) {
            read_size = sizeof(MBR) - mbr_offset;
        }

        memcpy(buffer, addr, read_size);
    } else {
        const uint32_t part_offset = ((lba - 1) * DISK_BLOCK_SIZE) + offset;
        const uint16_t sector_id = part_offset / FLASH_SECTOR_SIZE;
        const uint32_t sector_offset = part_offset - (sector_id * FLASH_SECTOR_SIZE); // part_offset % FLASH_SECTOR_SIZE


        const uint8_t* addr = fs::FLASH_START + part_offset;
        if (sector_id == cached_sector_id) {
            if (sector_offset + bufsize > FLASH_SECTOR_SIZE) {
                read_size = FLASH_SECTOR_SIZE - sector_offset;
            }
            addr = cached_sector + sector_offset;
        }

        memcpy(buffer, addr, read_size);
    }

    return (int32_t) read_size;
}

bool tud_msc_is_writable_cb(uint8_t lun)
{
    (void) lun;

    return Macropad::get_instance().EnableMSC;
}

// Callback invoked when received WRITE10 command.
// Process data in buffer to disk's storage and return number of written bytes
int32_t tud_msc_write10_cb(uint8_t lun, uint32_t lba, uint32_t offset, uint8_t* buffer, uint32_t bufsize)
{
    (void) lun;

    // out of ramdisk
    if (lba >= DISK_BLOCK_NUM || lba == 0) {
        return -1;
    }

    const uint32_t part_offset = ((lba - 1) * DISK_BLOCK_SIZE) + offset;
    const uint16_t sector_id = part_offset / FLASH_SECTOR_SIZE;
    const uint32_t sector_offset = part_offset - (sector_id * FLASH_SECTOR_SIZE); // part_offset % FLASH_SECTOR_SIZE

    if (cached_sector_id != sector_id) {
        if (cached_sector_id >= 0) {
            uint32_t ints = save_and_disable_interrupts();
            flash_range_erase(fs::FLASH_OFFSET + (FLASH_SECTOR_SIZE * cached_sector_id), FLASH_SECTOR_SIZE);
            flash_range_program(fs::FLASH_OFFSET + (FLASH_SECTOR_SIZE * cached_sector_id), cached_sector, FLASH_SECTOR_SIZE);
            restore_interrupts(ints);
        }

        memcpy(cached_sector, fs::FLASH_START + (FLASH_SECTOR_SIZE * sector_id), FLASH_SECTOR_SIZE);
        cached_sector_id = sector_id;
    }

    uint32_t write_size = bufsize;
    if (sector_offset + write_size > FLASH_SECTOR_SIZE) {
        write_size = FLASH_SECTOR_SIZE - sector_offset;
    }
    memcpy(cached_sector + sector_offset, buffer, write_size);

    return (int32_t) write_size;
}

void tud_msc_write10_complete_cb(uint8_t lun)
{
    if (cached_sector_id >= 0) {
        uint32_t ints = save_and_disable_interrupts();
        flash_range_erase(fs::FLASH_OFFSET + (FLASH_SECTOR_SIZE * cached_sector_id), FLASH_SECTOR_SIZE);
        flash_range_program(fs::FLASH_OFFSET + (FLASH_SECTOR_SIZE * cached_sector_id), cached_sector, FLASH_SECTOR_SIZE);
        restore_interrupts(ints);
    }
}

// Callback invoked when received an SCSI command not in built-in list below
// - READ_CAPACITY10, READ_FORMAT_CAPACITY, INQUIRY, MODE_SENSE6, REQUEST_SENSE
// - READ10 and WRITE10 has their own callbacks
int32_t tud_msc_scsi_cb(uint8_t lun, uint8_t const scsi_cmd[16], void* buffer, uint16_t bufsize)
{
    // read10 & write10 has their own callback and MUST not be handled here

    void const* response = NULL;
    int32_t resplen = 0;

    // most scsi handled is input
    bool in_xfer = true;

    switch (scsi_cmd[0]) {
        case SCSI_CMD_PREVENT_ALLOW_MEDIUM_REMOVAL:
            // Host is about to read/write etc ... better not to disconnect disk
            resplen = 0;
            break;

        default:
            // Set Sense = Invalid Command Operation
            tud_msc_set_sense(lun, SCSI_SENSE_ILLEGAL_REQUEST, 0x20, 0x00);

            // negative means error -> tinyusb could stall and/or response with failed status
            resplen = -1;
            break;
    }

    // return resplen must not larger than bufsize
    if (resplen > bufsize) {
        resplen = bufsize;
    }

    if (response && (resplen > 0)) {
        if (in_xfer) {
            memcpy(buffer, response, (size_t) resplen);
        } else {
            // SCSI output
        }
    }

    return (int32_t) resplen;
}

}

