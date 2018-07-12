#include "timer.h"
#include "isr.h"
#include "ports.h"
#include "../../common.h"
#include "../../../drivers/screen.h"

uint32_t tick = 0;

static void timer_callback(registers_t *regs){
  tick++;
  char c[128] = "";
  int_to_ascii(tick, c);
  uint8_t backup = get_color();
  set_color(VGA_COLOR_CYAN, VGA_COLOR_BLACK);
  kprint_at(c, 75, 0, 1);
  set_raw_color(backup);
  UNUSED(regs);
}

void init_timer(uint32_t freq){
  register_interrupt_handler(IRQ0, timer_callback);

  uint32_t divisor = 1193180 / freq;
  uint8_t low = (uint8_t)(divisor & 0xFF);
  uint8_t high = (uint8_t)((divisor >> 8) & 0xFF);

  outb(0x43, 0x36);
  outb(0x40, low);
  outb(0x40, high);
}
