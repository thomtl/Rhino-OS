#ifndef CMOS_h
#define CMOS_h

#include <stdint.h>
#include "ports.h"
#define NMI_DISABLE_BIT 0x80
#define CMOS_REG_SELECT 0x70
#define CMOS_REG_DATA 0x71
uint8_t read_cmos(uint8_t reg);
void write_cmos(uint8_t reg, uint8_t val);

#endif