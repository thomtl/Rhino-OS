#include "cmos.h"

uint8_t read_cmos(uint8_t reg){
    __asm__ __volatile__ ("cli");
    outb(CMOS_REG_SELECT, (CMOS_NMI_DISABLE_BIT << 7) | reg);
    uint8_t ret = inb(CMOS_REG_DATA);
    __asm__ __volatile__ ("sti");
    return ret;
}

void write_cmos(uint8_t reg, uint8_t val){
    __asm__ __volatile__ ("cli");
    outb(CMOS_REG_SELECT, (CMOS_NMI_DISABLE_BIT << 7) | reg);
    outb(CMOS_REG_DATA, val);
    __asm__ __volatile__ ("sti");
}
