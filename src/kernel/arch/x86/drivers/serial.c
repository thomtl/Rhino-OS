#include <rhino/arch/x86/drivers/serial.h>

void serial_configure_baud_rate(uint16_t com, uint16_t divisor){
  outb(SERIAL_LINE_COMMAND_PORT(com), SERIAL_LINE_ENABLE_DLAB);
  outb(SERIAL_DATA_PORT(com), (divisor >> 8) & 0x00ff);
  outb(SERIAL_DATA_PORT(com), divisor & 0x00ff);
}
void serial_configure_line(uint16_t com){
  /* Bit:     | 7 | 6 | 5 4 3 | 2 | 1 0 |
     Content: | d | b | prty  | s | dl  |
     Value:   | 0 | 0 | 0 0 0 | 0 | 1 1 | = 0x03
  */
  outb(SERIAL_LINE_COMMAND_PORT(com), 0x03);
}
uint32_t serial_is_transmit_fifo_empty(uint32_t com){
  return inb(SERIAL_LINE_STATUS_PORT(com)) & 0x20;
}
void serial_write(uint32_t com, char a){
  while (serial_is_transmit_fifo_empty(com) == 0);

  outb(com, a);
}
uint32_t serial_received(uint32_t com){
  return inb(SERIAL_LINE_STATUS_PORT(com)) & 1;
}
char read_serial(uint32_t com){
  while(serial_received(com) == 0);

  return inb(com);
}
