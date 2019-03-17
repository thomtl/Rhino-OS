#include <rhino/arch/x86/timer.h>


volatile uint32_t irq0s = 0;
volatile uint32_t irq8s = 0;
volatile uint32_t seconds = 0;
time_t time = {0};

static void update_time(){
	time.second++;
    if(time.second == 60){
        time.minute++;
        time.second = 0;
    }

    if(time.minute == 60){
        time.hour++;
        time.minute = 0;
	}	

    if(time.hour == 24){
        time.day++;
        time.hour = 0;
    }

	if(time.day > 27){ // leap year
        // Don't even try
        time = read_rtc_time();
  }
}


bool hpet;
static void irq0_callback(registers_t *regs){
	irq0s++;
  	if(irq0s % 1000 == 0) {
		seconds++;

		update_time();	
  	}
  
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
		hpet = false;//init_hpet(100000);
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

	time = read_rtc_time();
	
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

time_t now(){
	return time;
}