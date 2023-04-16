#include <stdio.h>
#include <string.h>

#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include "picoi2c.h"
#include "si5351.h"

int stdin_char;
uint8_t i2c_buffer;
int buffer_counter = 0;
char stdin_buffer[256] = {0};
struct Si5351Config config = {0};

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

int main() {
    stdio_init_all();
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

    setup_i2c();
    config.ref_freq = REF_25;

    while (!stdio_usb_connected()) continue;
    read_from_buffer();
    printf("Pico53\n\n");

    while (true) {
        read_from_buffer();
        if (strcmp(stdin_buffer, "i2c") == 0) {
            if (i2c_read_blocking(i2c_default, 0x60, &i2c_buffer, 1, false) == PICO_ERROR_GENERIC) {
                printf("Error connecting\n");
            } else {
                printf("I2C device found on 0x60\n");
            }
        }
        if (strncmp(stdin_buffer, "freq", 4) == 0) {
            si5351_init(0x60, i2c_default);
            if (si5351_gen_conf(&config, 151e6) == 0) {
                si5351_set_freq(&config, MS0);
                si5351_set_out(MS0, 1);
            } else {
                printf("Could not generate a valid configuration. No frequency will be set\n");
            }
            printf("FMD = %lu + (%lu / %lu)\n", config.fmd_int, config.fmd_num, config.fmd_den);
            printf("OMD = %lu + (%lu / %lu)\n", config.omd_int, config.omd_num, config.omd_den);
        }
        printf("\n\n");
    }
}
