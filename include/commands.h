#ifndef PICO53_COMMANDS_H
#define PICO53_COMMANDS_H

#include <stdlib.h>
#include <string.h>

#include "si5351.h"

enum Command {
    COMMAND_STATUS,
    COMMAND_INIT,
    COMMAND_SET_FREQ,
    COMMAND_READ,
    COMMAND_WRITE,
    COMMAND_ENABLE,
    COMMAND_DISABLE,
    COMMAND_UNKNOWN
};

struct FreqCommand {
    enum Multisynth multisynth;
    double frequency;
    uint8_t invert;
};

struct WriteCommand {
    uint8_t reg;
    uint8_t value;
};

enum Command get_command(const char *buffer);
uint8_t command_init(const char *buffer, uint8_t *address);
uint8_t command_set_freq(const char *buffer, struct FreqCommand *args);
uint8_t command_read(const char *buffer, uint8_t *reg);
uint8_t command_write(const char *buffer, struct WriteCommand *args);
uint8_t command_enable(const char *buffer, uint8_t *ms);
uint8_t command_disable(const char *buffer, uint8_t *ms);

#endif
