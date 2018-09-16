#include <rhino/arch/x86/timer.h>
#include <rhino/arch/x86/isr.h>
#include <rhino/arch/x86/ports.h>
#include <rhino/common.h>
#include <rhino/multitasking/scheduler.h>

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

static void play_sound(uint32_t frq){
  uint32_t div;
  uint8_t tmp;

  div = 1193180 / frq;
  outb(0x43, 0xb6);
  outb(0x42, (uint8_t)(div));
  outb(0x42, (uint8_t)(div >> 8));

  tmp = inb(0x61);
  if(tmp != (tmp | 3)) outb(0x61, tmp | 3);
}

static void total_silence(){
  uint8_t tmp = inb(0x61) & 0xFC;
  outb(0x61, tmp);
}

void beep(){
  play_sound(1000);
  for(int i = 0; i < 999999; i++);
  total_silence();
}