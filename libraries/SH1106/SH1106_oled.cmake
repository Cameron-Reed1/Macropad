add_library(SH1106_oled INTERFACE)

target_sources(SH1106_oled INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/SH1106_SPI.cpp
  ${CMAKE_CURRENT_LIST_DIR}/Print.cpp
)

target_include_directories(SH1106_oled INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)

# Pull in pico libraries that we need
target_link_libraries(SH1106_oled INTERFACE pico_stdlib hardware_spi)