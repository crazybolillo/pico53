add_library(picoi2c picoi2c.c)
target_link_libraries(picoi2c hardware_i2c si5351)

add_executable(pico53 pico53.c)
target_link_libraries(pico53 pico_stdlib pico_stdio_usb picoi2c)
pico_add_extra_outputs(pico53)
