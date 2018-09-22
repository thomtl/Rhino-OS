#pragma once

#include <rhino/common.h>
#define KERNEL_CS 0x08
// how every Interrupt gate handler is defined
typedef struct {
  uint16_t low_offset; // Lower 16bit of handler function address
  uint16_t sel; // Kernel segment selector
  uint8_t always0;
  /*
   *bit 7: "Interrupt is present"
   *bit 6-5: "Privilege level of caller (0=kernel..3=user)"
   *bit 4: "set to 0 for Interrupt gates"
   *bit 3-0: bits 1110 = decimal 14 = "32 bit Interrupt gate"
  */
  uint8_t flags;
  uint16_t high_offset; // Higher 16bit offset of handler function address
} __attribute__((packed)) idt_gate_t ;

/* A pointer to the array of Interrupt handlers
  Assembly instruction 'lidt' will read it*/
typedef struct {
  uint16_t limit;
  uint32_t base;
} __attribute__((packed)) idt_register_t ;

#define IDT_ENTRIES 256


void set_idt_gate(int n, uint32_t handler, uint16_t sel, uint8_t dpl);
void set_idt();