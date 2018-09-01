#pragma once

#include <stdint.h>

// BYTE IO - 8bits
unsigned char inb (uint16_t port);
void outb (uint16_t port, uint8_t data);

// WORD IO - 16bits
unsigned short inw (uint16_t port);
void outw (uint16_t port, uint16_t data);

// DWORD IO - 32bits
uint32_t ind(uint16_t port);
void outd(uint16_t port, uint32_t data);
