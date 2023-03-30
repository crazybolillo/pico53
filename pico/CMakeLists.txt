add_executable(pico53 pico53.c)
target_link_libraries(pico53 pico_stdlib pico_stdio_usb hardware_i2c)
pico_add_extra_outputs(pico53)
