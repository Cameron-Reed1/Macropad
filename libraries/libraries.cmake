add_library(littlefs INTERFACE)
target_sources(littlefs INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/littlefs/lfs.c
    ${CMAKE_CURRENT_LIST_DIR}/littlefs/lfs_util.c
)
target_include_directories(littlefs INTERFACE ${CMAKE_CURRENT_LIST_DIR}/littlefs)

include("libraries/pico_neopixels/pico_neopixel.cmake")
include("libraries/SH1106/SH1106_oled.cmake")
