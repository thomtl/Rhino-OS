#ifndef KMALLOC_H
#define KMALLOC_H
#include <stdint.h>
#include <stddef.h>
void initialize_memory_manager();
void* kmalloc(size_t size);
#endif
