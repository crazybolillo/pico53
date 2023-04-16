#include "picoi2c.h"

static uint8_t address;
static struct i2c_inst *i2c;
static uint8_t i2c_buffer[16];

static void si5351_write(size_t size) { i2c_write_blocking(i2c, address, i2c_buffer, size, false); }

void si5351_init(uint8_t si5351_addr, i2c_inst_t *i2c_interface) {
    address = si5351_addr;
    i2c = i2c_interface;

    i2c_buffer[0] = 0x03;
    i2c_buffer[1] = 0xFF;
    si5351_write(2);

    i2c_buffer[0] = 0x10;
    i2c_buffer[1] = CLK0_CTRL_DEFAULT;
    i2c_buffer[2] = CLK1_CTRL_DEFAULT;
    i2c_buffer[3] = CLK2_CTRL_DEFAULT;
    si5351_write(4);

    i2c_buffer[0] = 0x95;
    i2c_buffer[1] = 0x00;
    si5351_write(2);

    i2c_buffer[0] = 0xA2;
    i2c_buffer[1] = 0x00;
    i2c_buffer[2] = 0x00;
    i2c_buffer[3] = 0x00;
    si5351_write(4);
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
    uint8_t output_en_ctrl;
    switch (ms) {
        case MS0:
            i2c_buffer[0] = 0x10;
            i2c_buffer[1] = CLK0_CTRL_DEFAULT & 0x7F;
            output_en_ctrl = ~(uint8_t)0x01;
            break;
        case MS1:
            i2c_buffer[0] = 0x11;
            i2c_buffer[1] = CLK1_CTRL_DEFAULT & 0x7F;
            output_en_ctrl = ~(uint8_t)0x02;
            break;
        case MS2:
            i2c_buffer[0] = 0x12;
            i2c_buffer[1] = CLK2_CTRL_DEFAULT & 0x7F;
            output_en_ctrl = ~(uint8_t)0x04;
    }

    i2c_write_blocking(i2c, address, i2c_buffer, 2, false);

    i2c_buffer[0] = 0x03;
    i2c_buffer[1] = output_en_ctrl;
    i2c_write_blocking(i2c, address, i2c_buffer, 2, false);
}
