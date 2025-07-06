#include <stdint.h>


namespace fs {

extern const uint32_t FLASH_OFFSET;
extern const uint8_t* FLASH_START;
extern const uint32_t SIZE;

void init();

} // namespace fs
