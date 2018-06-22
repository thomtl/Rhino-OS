#ifndef SERIAL_H
#define SERIAL_H
#include "../ports.h"
#define SERIAL_COM1_BASE 0x3F8
#define SERIAL_DATA_PORT(base) ((base))
#define SERIAL_FIFO_COMMAND_PORT(base) ((base) + 2)
#define SERIAL_LINE_COMMAND_PORT(base) ((base) + 3)
#define SERIAL_MODEM_COMMAND_PORT(base) ((base) + 4)
#define SERIAL_LINE_STATUS_PORT(base) ((base) + 5)
#endif