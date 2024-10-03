add_library(pico_lcd_i2c INTERFACE)

target_include_directories(pico_lcd_i2c
    INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(pico_lcd_i2c
    INTERFACE
    hardware_i2c
)

target_sources(pico_lcd_i2c
    INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/lcd_i2c.c
)
