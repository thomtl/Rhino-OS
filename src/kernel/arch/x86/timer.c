#include <rhino/arch/x86/timer.h>
#include <rhino/arch/x86/isr.h>
#include <rhino/arch/x86/io.h>
#include <rhino/common.h>
#include <rhino/multitasking/scheduler.h>
#include <rhino/arch/x86/drivers/pit.h>
#include <rhino/arch/x86/drivers/hpet.h>

volatile uint32_t irq0s = 0;
volatile uint32_t irq8s = 0;
volatile uint32_t seconds = 0;
bool hpet;
static void irq0_callback(registers_t *regs){
  irq0s++;
  if(irq0s % 1000 == 0) seconds++;
  
  schedule(regs);
}

static void irq8_callback(registers_t *regs){
  irq8s++;
  UNUSED(regs);
}

void init_timer(){
  register_interrupt_handler(IRQ0, irq0_callback);
  register_interrupt_handler(IRQ8, irq8_callback);

  debug_log("[TIMER]: Initializing Timer\n");
  if(detect_hpet()){
    hpet = init_hpet(100000);
    if(hpet){
     debug_log("[TIMER]: Using HPET as main Timer\n");
    } else {
      goto pit;
    }
  } else {
    pit:
    init_pit(1000);
    debug_log("[TIMER]: Using PIT as main Timer\n");
  }
  debug_log("[TIMER]: Initialized Timer\n");
}

uint32_t get_tick(){
  return irq0s;
}

void sleep(uint32_t nSeconds){
  volatile uint32_t eticks = nSeconds + seconds;
  uint32_t eflags = save_eflags();
  STI();
  while(seconds < eticks) asm("hlt");
  load_eflags(eflags);
}

void msleep(uint32_t nMseconds){
  volatile uint32_t eticks = nMseconds + irq0s;
  uint32_t eflags = save_eflags();
  STI();
  while(irq0s < eticks) asm("hlt");
  load_eflags(eflags);
}