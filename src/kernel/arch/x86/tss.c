#include <rhino/arch/x86/tss.h>

tss_t tss;

void set_tss(){
    debug_log("[TSS]: Initializing TSS\n");
    tss.ss0 = 0x10;
    tss.esp0 = (uint32_t)&stack_top;
    tss.iomap_base = (uint16_t)sizeof(tss_t);
    uint32_t addr=(uint32_t)&tss; 
	uint32_t size = sizeof(tss_t)+1;
    gdt_set_gate(5,addr,addr+size,0x89,0xCF);
    debug_log("[TSS]: TSS Initialized\n");
}

tss_t* get_tss(){
    return &tss;
}