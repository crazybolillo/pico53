#ifndef PICO53_SI5351_H
#define PICO53_SI5351_H

#include <stdint.h>

struct Si5351Config {
    uint32_t pll_numerator;
    uint32_t pll_denominator;
    uint8_t pll_multiplier;
};

#endif
