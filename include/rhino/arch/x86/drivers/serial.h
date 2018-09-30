#pragma once

#include <rhino/arch/x86/io.h>

#define SERIAL_COM1_BASE 0x3F8
#define SERIAL_DATA_PORT(base) ((base))
#define SERIAL_FIFO_COMMAND_PORT(base) ((base) + 2)
#define SERIAL_LINE_COMMAND_PORT(base) ((base) + 3)
#define SERIAL_MODEM_COMMAND_PORT(base) ((base) + 4)
#define SERIAL_LINE_STATUS_PORT(base) ((base) + 5)

#define SERIAL_LINE_ENABLE_DLAB 0x80

#define COM1 SERIAL_COM1_BASE

void serial_configure_baud_rate(uint16_t com, uint16_t divisor);
void serial_configure_line(uint16_t com);
uint32_t serial_is_transmit_fifo_empty(uint32_t com);
uint32_t serial_received();
char read_serial(uint32_t com);
void serial_write(uint32_t com, char a);