#include <rhino/arch/x86/drivers/pcspkr.h>

static void pcspkr_set_freq(uint32_t hz){
    uint32_t div = 1193189 / hz;
    outb(0x43, 0xB6);
    outb(0x42, div & 0xFF);
    outb(0x42, div >> 8);
}

void beep(uint32_t ticks){
    uint8_t playing = inb(0x61);
    uint8_t stop = inb(0x61) & 0xFC;
    uint8_t start = playing | 3;
    pcspkr_set_freq(44100);

    if(start != playing){
        outb(0x61, playing);
    }
    ticks++;
    for(volatile int i = 0; i < 1000; i++);
    outb(0x61, stop);
}