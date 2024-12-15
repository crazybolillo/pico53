#include "commands.h"

enum Command get_command(const char *buffer) {
    if (strncmp(buffer, "status", 6) == 0) {
        return COMMAND_STATUS;
    } else if (strncmp(buffer, "init", 4) == 0) {
        return COMMAND_INIT;
    } else if (strncmp(buffer, "freq", 4) == 0) {
        return COMMAND_SET_FREQ;
    } else if (strncmp(buffer, "read", 4) == 0) {
        return COMMAND_READ;
    } else if (strncmp(buffer, "write", 5) == 0) {
        return COMMAND_WRITE;
    } else if (strncmp(buffer, "enable", 6) == 0) {
        return COMMAND_ENABLE;
    } else if (strncmp(buffer, "disable", 7) == 0) {
        return COMMAND_DISABLE;
    } else {
        return COMMAND_UNKNOWN;
    }
}

uint8_t command_init(const char *buffer, uint8_t *address) {
    char *end;
    buffer += 4;
    *address = strtol(buffer, &end, 16);

    if (buffer == end) { *address = 0x60; }

    return 0;
}

uint8_t command_set_freq(const char *buffer, struct FreqCommand *args) {
    char *end;
    buffer += 4;

    args->multisynth = strtol(buffer, &end, 10);
    if (buffer == end) { return 1; }
    if (args->multisynth > 2) { return 1; }

    buffer = end;
    args->frequency = strtod(buffer, &end);

    if (buffer != end) {
        if (strcmp(" inv", end) == 0) {
            args->invert = 1;
        } else {
            args->invert = 0;
        }
    }

    return buffer == end;
}

uint8_t command_read(const char *buffer, uint8_t *reg) {
    char *end;
    buffer += 4;

    *reg = strtol(buffer, &end, 16);

    return buffer == end;
}

uint8_t command_write(const char *buffer, struct WriteCommand *args) {
    char *end;
    buffer += 5;

    args->reg = strtol(buffer, &end, 16);
    if (buffer == end) { return 1; }

    buffer = end;
    args->value = strtol(buffer, &end, 16);

    return buffer == end;
}

uint8_t command_enable(const char *buffer, uint8_t *ms) {
    char *end;
    buffer += 6;

    *ms = strtol(buffer, &end, 16);
    if (*ms > 2) { return 1; }

    return buffer == end;
}

uint8_t command_disable(const char *buffer, uint8_t *ms) {
    char *end;
    buffer += 7;

    *ms = strtol(buffer, &end, 16);
    if (*ms > 2) { return 1; }

    return buffer == end;
}
