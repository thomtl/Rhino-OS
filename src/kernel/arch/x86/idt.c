#include <rhino/arch/x86/idt.h>
#include <rhino/arch/x86/type.h>

idt_gate_t idt[IDT_ENTRIES];
idt_register_t idt_reg;

void set_idt_gate(int n, uint32_t handler, uint16_t sel, uint8_t dpl){
  uint8_t flags = 0;
  BIT_CLEAR(flags, 0);
  BIT_SET(flags, 1);
  BIT_SET(flags, 2);
  BIT_SET(flags, 3);
  BIT_CLEAR(flags, 4);
  if(dpl == 0){
    BIT_CLEAR(flags, 5);
    BIT_CLEAR(flags, 6);
  } else if (dpl == 1) {
    BIT_SET(flags, 5);
    BIT_CLEAR(flags, 6);
  } else if(dpl == 2){
    BIT_CLEAR(flags, 5);
    BIT_SET(flags, 6);
  } else if(dpl == 3){
    BIT_SET(flags, 5);
    BIT_SET(flags, 6);
  }
  BIT_SET(flags, 7);
  idt[n].low_offset = low_16(handler);
  idt[n].sel = sel;
  idt[n].always0 = 0;
  idt[n].flags = flags;
  idt[n].high_offset = high_16(handler);
}
void set_idt(){
  idt_reg.base = (uint32_t) &idt;
  idt_reg.limit = IDT_ENTRIES * sizeof(idt_gate_t) - 1;

  asm("lidtl (%0)" : : "r" (&idt_reg));
}
