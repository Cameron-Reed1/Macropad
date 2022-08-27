include(pico_sdk_import.cmake)

pico_sdk_init()

add_library(pico_neopixel INTERFACE)

target_sources(pico_neopixel INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/Adafruit_NeoPixel.cpp
)

pico_generate_pio_header(pico_neopixel ${CMAKE_CURRENT_LIST_DIR}/ws2812byte.pio)

target_include_directories(pico_neopixel INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)

# Pull in pico libraries that we need
target_link_libraries(pico_neopixel INTERFACE pico_stdlib hardware_pio pico_malloc pico_mem_ops)