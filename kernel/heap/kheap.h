#ifndef KMALLOC_H
#define KMALLOC_H
#include <stdint.h>
#include <stddef.h>
void initialize_memory_manager();
void* kmalloc(size_t size);
void* kmalloc_a(size_t size);
void* kmalloc_p(size_t size, uint32_t* phys);
void* kmalloc_ap(size_t size, uint32_t* phys);
#endif
