#include <stdio.h>
#include <string.h>

#include "commands.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include "picoi2c.h"
#include "si5351.h"

#define INVALID_ARGUMENTS()                            \
    printf("Incorrect arguments, please try again\n"); \
    continue

int stdin_char;
int buffer_counter = 0;
char stdin_buffer[256] = {0};

uint8_t i2c_reg = 0;
uint8_t ms = 0;
int i2c_val = 0;
uint8_t i2c_is_init = 0;
uint8_t i2c_address = 0;

enum Command command;
struct FreqCommand freq_args;
struct WriteCommand write_args;
struct Si5351Config config = {0};

void read_from_buffer() {
    memset(stdin_buffer, 0, sizeof(stdin_buffer));
    buffer_counter = 0;
    printf("$> ");
    while (buffer_counter < 255) {
        stdin_char = getchar_timeout_us(1000);
        if (stdin_char == PICO_ERROR_TIMEOUT) { continue; }

        if (stdin_char == 0x08) {
            if (buffer_counter > 0) {
                printf("\b \b");
                stdin_buffer[buffer_counter] = '\0';
                buffer_counter--;
            }
            continue;
        }
        if (stdin_char == 13) {
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
    setup_i2c();
    config.ref_freq = REF_25;

    while (!stdio_usb_connected()) continue;
    read_from_buffer();
    printf("Pico53\n");

    while (true) {
        printf("\n");
        read_from_buffer();
        command = get_command(stdin_buffer);
        if ((i2c_is_init == 0) && (command != COMMAND_INIT) && (command != COMMAND_UNKNOWN)) {
            printf("Initialization is still pending. Please run the 'init' command\n");
            continue;
        }

        switch (command) {
            case COMMAND_STATUS:
                if (si5351_read(0x00) == PICO_ERROR_GENERIC) {
                    printf("Could not find Si5351 at address 0x%02X\n", i2c_address);
                    continue;
                }
                printf("Si5351 found at address 0x%02X\n", i2c_address);
                break;
            case COMMAND_INIT:
                if (command_init(stdin_buffer, &i2c_address) != 0) {
                    printf("Error, please provide a valid I2C address\n");
                    continue;
                }
                printf("Initializing Si5351 at 0x%02X\n", i2c_address);
                si5351_init(i2c_address, i2c_default);
                i2c_is_init = 1;
                break;
            case COMMAND_SET_FREQ:
                if (command_set_freq(stdin_buffer, &freq_args) != 0) { INVALID_ARGUMENTS(); }
                if (si5351_gen_conf(&config, freq_args.frequency) != 0) {
                    printf("Could not generate a valid configuration. No frequency will be set\n");
                    continue;
                }
                printf(
                    "Setting Frequency %f Hz on CLK%d\n", freq_args.frequency, freq_args.multisynth
                );
                printf("FMD = %lu + (%lu / %lu)\n", config.fmd_int, config.fmd_num, config.fmd_den);
                printf("OMD = %lu + (%lu / %lu)\n", config.omd_int, config.omd_num, config.omd_den);
                si5351_set_freq(&config, freq_args.multisynth);
                si5351_set_out(freq_args.multisynth, 1);
                si5351_en_clk(freq_args.multisynth, 1, freq_args.invert);
                break;
            case COMMAND_READ:
                if (command_read(stdin_buffer, &i2c_reg) == 0) {
                    i2c_val = si5351_read(i2c_reg);
                    if (i2c_val == PICO_ERROR_GENERIC) {
                        printf("Failed to perform read, please try again\n");
                        continue;
                    }
                    printf("%02X => %02X\n", i2c_reg, (uint8_t)i2c_val);
                }
                break;
            case COMMAND_WRITE:
                if (command_write(stdin_buffer, &write_args) != 0) { INVALID_ARGUMENTS(); }
                si5351_write(write_args.reg, write_args.value);
                printf(
                    "Value 0x%02X written to register 0x%02X\n", write_args.value, write_args.reg
                );
                break;
            case COMMAND_ENABLE:
                if (command_enable(stdin_buffer, &i2c_reg) != 0) { INVALID_ARGUMENTS(); }
                si5351_en_clk(i2c_reg, 1, 0);
                si5351_set_out(i2c_reg, 1);
                break;
            case COMMAND_DISABLE:
                if (command_disable(stdin_buffer, &i2c_reg) != 0) { INVALID_ARGUMENTS(); }
                si5351_en_clk(i2c_reg, 0, 0);
                si5351_set_out(i2c_reg, 0);
                break;
            case COMMAND_UNKNOWN:
                printf("Unknown command\n");
                break;
        }
    }
}
