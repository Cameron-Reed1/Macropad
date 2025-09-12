add_library(littlefs INTERFACE)

target_sources(littlefs INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/littlefs/lfs.c
    ${CMAKE_CURRENT_LIST_DIR}/littlefs/lfs_util.c
)

target_include_directories(littlefs INTERFACE ${CMAKE_CURRENT_LIST_DIR}/littlefs)



add_library(pico_neopixel INTERFACE)

target_sources(pico_neopixel INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/neopixels/pico_neopixels/Adafruit_NeoPixel.cpp
)

pico_generate_pio_header(pico_neopixel ${CMAKE_CURRENT_LIST_DIR}/neopixels/pico_neopixels/ws2812byte.pio)

target_include_directories(pico_neopixel INTERFACE ${CMAKE_CURRENT_LIST_DIR}/neopixels/pico_neopixels/include)

# Pull in pico libraries that we need
target_link_libraries(pico_neopixel INTERFACE pico_stdlib hardware_pio pico_malloc pico_mem_ops)



include("libraries/SH1106/SH1106_oled.cmake")
