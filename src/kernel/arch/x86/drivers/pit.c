#include <rhino/arch/x86/drivers/pit.h>

void init_pit(uint32_t frq){
    uint32_t divisor = PIT_DIVISOR / frq;
    uint8_t low = (uint8_t)(divisor & 0xFF);
    uint8_t high = (uint8_t)((divisor >> 8));

    outb(PIT_CMD, 0x36); // Channel 0, Low / High byte, Square wave
    outb(PIT_DAT_CH_0, low);
    outb(PIT_DAT_CH_0, high);
    debug_log("[PIT]: PIT Started\n");
}