#include "picoi2c.h"

static uint8_t address;
static struct i2c_inst *i2c;
static uint8_t i2c_buffer[16];

static void write(size_t size) { i2c_write_blocking(i2c, address, i2c_buffer, size, false); }

void si5351_init(uint8_t si5351_addr, i2c_inst_t *i2c_interface) {
    address = si5351_addr;
    i2c = i2c_interface;

    i2c_buffer[0] = Si5351_OUT_EN_CTRL;
    i2c_buffer[1] = 0xFF;
    write(2);

    i2c_buffer[0] = Si5351_CLK0_CTRL;
    i2c_buffer[1] = CLK0_CTRL_DEFAULT;
    i2c_buffer[2] = CLK1_CTRL_DEFAULT;
    i2c_buffer[3] = CLK2_CTRL_DEFAULT;
    write(4);

    i2c_buffer[0] = 0x95;
    i2c_buffer[1] = 0x00;
    write(2);

    i2c_buffer[0] = 0xA2;
    i2c_buffer[1] = 0x00;
    i2c_buffer[2] = 0x00;
    i2c_buffer[3] = 0x00;
    write(4);
}

void si5351_set_freq(struct Si5351Config *config, enum Multisynth ms) {
    enum PLL pll;
    if (ms != MS2) {
        pll = PLL_A;
    } else {
        pll = PLL_B;
    }

    si5351_comp_fmd_regs(config, pll, i2c_buffer);
    i2c_write_blocking(i2c, address, i2c_buffer, 9, false);

    si5351_comp_omd_regs(config, ms, i2c_buffer);
    i2c_write_blocking(i2c, address, i2c_buffer, 9, false);
}

void si5351_set_out(enum Multisynth ms, uint8_t status) {
    uint8_t config = si5351_read(Si5351_OUT_EN_CTRL);

    i2c_buffer[0] = 0x03;
    if (status != 0) {
        config &= (~(1 << ms));
    } else {
        config |= (1 << ms);
    }

    si5351_write(Si5351_OUT_EN_CTRL, config);
}

void si5351_en_clk(enum Multisynth ms, uint8_t status, uint8_t invert) {
    uint8_t config_addr;
    uint8_t config;

    switch (ms) {
        case MS0:
            config_addr = Si5351_CLK0_CTRL;
            break;
        case MS1:
            config_addr = Si5351_CLK1_CTRL;
            break;
        case MS2:
            config_addr = Si5351_CLK2_CTRL;
            break;
    }

    config = si5351_read(config_addr);
    if (status != 0) {
        config &= 0x7F;
    } else {
        config |= 0x80;
    }
    if (invert != 0) {
        config |= 0x10;
    } else {
        config &= 0xEF;
    }

    si5351_write(config_addr, config);
}

int si5351_write(uint8_t reg, uint8_t value) {
    i2c_buffer[0] = reg;
    i2c_buffer[1] = value;

    return i2c_write_blocking(i2c, address, i2c_buffer, 2, false);
}

int si5351_read(uint8_t reg) {
    int status;
    i2c_buffer[0] = reg;

    i2c_write_blocking(i2c, address, i2c_buffer, 1, true);
    status = i2c_read_blocking(i2c, address, i2c_buffer, 1, false);

    if (status != PICO_ERROR_GENERIC) { return i2c_buffer[0]; }

    return PICO_ERROR_GENERIC;
}
