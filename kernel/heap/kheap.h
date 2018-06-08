#ifndef KMALLOC_H
#define KMALLOC_H
#include <stdint.h>
#include <stddef.h>
extern uint32_t end;
uint32_t kmalloc_int(uint32_t sz, int align, uint32_t *phys);

uint32_t kmalloc_a(uint32_t sz);

uint32_t kmalloc_p(uint32_t sz, uint32_t *phys);

uint32_t kmalloc_ap(uint32_t sz, uint32_t *phys);

uint32_t kmalloc(uint32_t sz);
#endif
