#include <rhino/arch/x86/gdt.h>
#include <rhino/arch/x86/tss.h>

gdt_entry gdt[6];
gdt_ptr gdtptr;

extern void gdt_flush();
void gdt_set_gate(uint32_t n, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity){
  gdt[n].base_low = (base & 0xFFFF);
  gdt[n].base_middle = (base >> 16) & 0xFF;
  gdt[n].base_high = (base >> 24) & 0xFF;

  gdt[n].limit_low = (limit & 0xFFFF);
  gdt[n].granularity = ((limit >> 16) & 0x0F);

  gdt[n].granularity |= (granularity & 0xF0);
  gdt[n].access = access;
  return;
}

void gdt_install(){
  debug_log("[GDT]: Initializing GDT\n");
  gdtptr.limit = (sizeof(gdt_entry) * 6) - 1;
  gdtptr.base = (uint32_t)&gdt;

  gdt_set_gate(0,0,0,0,0);
  gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
  gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);

  gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);
  gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);
  set_tss();

  gdt_flush();
  tss_flush();
  debug_log("[GDT]: GDT Initialized\n");
  return;
}
