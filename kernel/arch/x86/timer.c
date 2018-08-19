#include "timer.h"
#include "isr.h"
#include "ports.h"
#include "../../common.h"
#include "drivers/screen.h"
#include "../../multitasking/scheduler.h"

uint32_t tick = 0;

static void timer_callback(registers_t *regs){
  tick++;
  UNUSED(regs);

  schedule(tick);
}

void init_timer(uint32_t freq){
  register_interrupt_handler(IRQ0, timer_callback);

  uint32_t divisor = 1193189 / freq;
  uint8_t low = (uint8_t)(divisor & 0xFF);
  uint8_t high = (uint8_t)((divisor >> 8));

  outb(0x43, 0x36);
  outb(0x40, low);
  outb(0x40, high);
}

uint32_t get_tick(){
  return tick;
}
