#pragma once

#include <rhino/arch/x86/io.h>
#include <rhino/arch/x86/isr.h>
#define SERIAL_COM1_BASE 0x3F8
#define SERIAL_COM2_BASE 0x2F8
#define SERIAL_COM3_BASE 0x3E8
#define SERIAL_COM4_BASE 0x2E8
#define SERIAL_DATA_PORT(base) ((base))
#define SERIAL_INTERRUPT_ENABLE_PORT(base) ((base) + 1)
#define SERIAL_FIFO_COMMAND_PORT(base) ((base) + 2)
#define SERIAL_LINE_COMMAND_PORT(base) ((base) + 3)
#define SERIAL_MODEM_COMMAND_PORT(base) ((base) + 4)
#define SERIAL_LINE_STATUS_PORT(base) ((base) + 5)

#define SERIAL_LINE_ENABLE_DLAB 7

#define COM1 SERIAL_COM1_BASE
#define COM2 SERIAL_COM2_BASE
#define COM3 SERIAL_COM3_BASE
#define COM4 SERIAL_COM4_BASE

char read_serial(uint32_t com);
void serial_write(uint32_t com, char a);
void init_serial();