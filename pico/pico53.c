#include <stdio.h>
#include <string.h>

#include "hardware/i2c.h"
#include "pico/stdlib.h"

int stdin_char;
uint8_t i2c_buffer;
int buffer_counter = 0;
char stdin_buffer[256] = {0};

uint8_t i2c_data_1[] = {0x03, 0x00};
uint8_t i2c_data_2[] = {0x1A, 0x01, 0xF4, 0x00, 0x0A, 0x3D, 0x00, 0x01, 0x5C};
uint8_t i2c_data_3[] = {
    0x2A, 0x00, 0x01, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00,
};
uint8_t i2c_data_4[] = {0x10, 0x4C};
uint8_t i2c_data_5[] = {0x95, 0x00};
uint8_t i2c_data_6[] = {0xA2, 0x00, 0x000, 0x00};

void read_from_buffer() {
    memset(stdin_buffer, 0, sizeof(stdin_buffer));
    buffer_counter = 0;
    printf("$> ");
    while (buffer_counter < 255) {
        stdin_char = getchar_timeout_us(1000);
        if (stdin_char == PICO_ERROR_TIMEOUT) {
            continue;
        } else if (stdin_char == 13) {
            printf("\n");
            break;
        }

        stdin_buffer[buffer_counter] = (char)stdin_char;
        printf("%c", stdin_buffer[buffer_counter]);
        buffer_counter++;
    }
}

void setup_i2c(void) {
    i2c_init(i2c_default, 100 * 1000);
    gpio_set_function(17, GPIO_FUNC_I2C);
    gpio_set_function(16, GPIO_FUNC_I2C);
    gpio_pull_up(17);
    gpio_pull_up(16);
}

void set_si5351a(void) {
    printf("Setting up Si5351A on 0x60\n");
    if (i2c_read_blocking(i2c_default, 0x60, &i2c_buffer, 1, false) == PICO_ERROR_GENERIC) {
        printf("Error connecting\n");
        return;
    }

    i2c_write_blocking(i2c_default, 0x60, i2c_data_2, sizeof(i2c_data_2), false);
    i2c_write_blocking(i2c_default, 0x60, i2c_data_3, sizeof(i2c_data_3), false);
    i2c_write_blocking(i2c_default, 0x60, i2c_data_1, sizeof(i2c_data_1), false);
    i2c_write_blocking(i2c_default, 0x60, i2c_data_4, sizeof(i2c_data_4), false);
    i2c_write_blocking(i2c_default, 0x60, i2c_data_5, sizeof(i2c_data_5), false);
    i2c_write_blocking(i2c_default, 0x60, i2c_data_6, sizeof(i2c_data_6), false);
}

int main() {
    stdio_init_all();
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

    setup_i2c();

    while (!stdio_usb_connected()) continue;
    read_from_buffer();
    printf("Pico53\n\n");

    while (true) {
        read_from_buffer();
        if (strcmp(stdin_buffer, "i2c") == 0) { set_si5351a(); }
        printf("\n\n");
    }
}
