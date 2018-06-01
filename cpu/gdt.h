#ifndef GDT_H
#define GDT_H
#include <stdint.h>
typedef struct {
  uint16_t limit_low;
  uint16_t base_low;
  uint8_t base_middle;
  uint8_t access;
  uint8_t granularity;
  uint8_t base_high;
} __attribute__((packed)) gdt_entry;

typedef struct {
  uint16_t limit;
  uint32_t base;
} __attribute__((packed)) gdt_ptr;

gdt_entry gdt[3];
gdt_ptr gdtptr;
void gdt_set_gate(uint32_t n, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity);
void gdt_install();
#endif
