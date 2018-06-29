#ifndef PORTS_H
#define PORTS_H

#include "type.h"
#include <stdint.h>
unsigned char inb (uint16_t port);
void outb (uint16_t port, uint8_t data);
unsigned short ins (uint16_t port);
void outs (uint16_t port, uint16_t data);
#endif
