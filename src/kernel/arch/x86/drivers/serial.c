#include <rhino/arch/x86/drivers/serial.h>

char com1_buf[32];
uint32_t com1_buf_index = 0;
uint32_t com1_read_index = 0;

char com2_buf[32];
uint32_t com2_buf_index = 0;
uint32_t com2_read_index = 0;

char com3_buf[32];
uint32_t com3_buf_index = 0;
uint32_t com3_read_index = 0;

char com4_buf[32];
uint32_t com4_buf_index = 0;
uint32_t com4_read_index = 0;

void com1_3_irq(registers_t* regs){
  uint8_t ir1 = inb(SERIAL_INTERRUPT_ENABLE_PORT(COM1));
  uint8_t ir3 = inb(SERIAL_INTERRUPT_ENABLE_PORT(COM3));
  if(BIT_IS_SET(ir1, 0)){
    if(!(inb(SERIAL_LINE_STATUS_PORT(COM1)) & 1)){
      kprint_err("[SERIAL]: COM1 sent IRQ but there is no data\n");
      goto com3;
    }
    com1_buf[com1_buf_index++] = inb(COM1);
  }
  com3:
  if(BIT_IS_SET(ir3, 0)){
    if(!(inb(SERIAL_LINE_STATUS_PORT(COM3)) & 1)){
      kprint_err("[SERIAL]: COM3 sent IRQ but there is no data\n");
      return;
    }
    com3_buf[com3_buf_index++] = inb(COM3);
  }
  UNUSED(regs);
}

void com2_4_irq(registers_t* regs){
  uint8_t ir2 = inb(SERIAL_INTERRUPT_ENABLE_PORT(COM2));
  uint8_t ir4 = inb(SERIAL_INTERRUPT_ENABLE_PORT(COM4));
  if(BIT_IS_SET(ir2, 0)){
    if(!(inb(SERIAL_LINE_STATUS_PORT(COM2)) & 1)){
      kprint_err("[SERIAL]: COM2 sent IRQ but there is no data\n");
      goto com4;
    }
    com2_buf[com2_buf_index++] = inb(COM2);
  }
  com4:
  if(BIT_IS_SET(ir4, 0)){
    if(!(inb(SERIAL_LINE_STATUS_PORT(COM4)) & 1)){
      kprint_err("[SERIAL]: COM4 sent IRQ but there is no data\n");
      return;
    }
    com4_buf[com4_buf_index++] = inb(COM4);
  }
  UNUSED(regs);
}

void serial_write(uint32_t com, char a){
  while ((inb(SERIAL_LINE_STATUS_PORT(com)) & 0x20) == 0);
  outb(com, a);
  while ((inb(SERIAL_LINE_STATUS_PORT(com)) & 0x20) == 0);
  outb(com, '\0');
}

uint32_t serial_received(uint32_t com){
  return inb(SERIAL_LINE_STATUS_PORT(com)) & 1;
}

char read_serial(uint32_t com){
  if(com == COM1){
    while(com1_buf_index <= com1_read_index);
    com1_read_index++;
    return com1_buf[com1_read_index];
  } else if(com == COM2){
    while(com2_buf_index <= com2_read_index);
    com2_read_index++;
    return com2_buf[com2_read_index];
  } else if(com == COM3){
    while(com3_buf_index <= com3_read_index);
    com3_read_index++;
    return com3_buf[com3_read_index];
  } else if(com == COM4){
    while(com4_buf_index <= com4_read_index);
    com4_read_index++;
    return com4_buf[com4_read_index];
  }
  return 0;
}

void serial_configure_baud_rate(uint16_t com, uint16_t divisor){
  uint8_t cmd = inb(SERIAL_LINE_COMMAND_PORT(com));
  BIT_SET(cmd, SERIAL_LINE_ENABLE_DLAB);
  outb(SERIAL_LINE_COMMAND_PORT(com), cmd);

  outb(SERIAL_DATA_PORT(com), (divisor >> 8) & 0x00ff);
  outb(SERIAL_DATA_PORT(com), divisor & 0x00ff);

  cmd = inb(SERIAL_LINE_COMMAND_PORT(com));
  BIT_CLEAR(cmd, SERIAL_LINE_ENABLE_DLAB);
  outb(SERIAL_LINE_COMMAND_PORT(com), cmd);
}

void init_serial(){
  register_interrupt_handler(IRQ4, com1_3_irq); // Install IRQ Handlers
  register_interrupt_handler(IRQ3, com2_4_irq);

  /* Bit:     | 7 | 6 | 5 4 3 | 2 | 1 0 |
     Content: | d | b | prty  | s | dl  |
     Value:   | 0 | 0 | 0 0 0 | 0 | 1 1 | = 0x03
  */

  outb(SERIAL_LINE_COMMAND_PORT(COM1), 0x03); // Configure COM1-4
  outb(SERIAL_LINE_COMMAND_PORT(COM2), 0x03);
  outb(SERIAL_LINE_COMMAND_PORT(COM3), 0x03);
  outb(SERIAL_LINE_COMMAND_PORT(COM4), 0x03);

  outb(SERIAL_INTERRUPT_ENABLE_PORT(COM1), 1); // Enable Interrupts COM1-4
  outb(SERIAL_INTERRUPT_ENABLE_PORT(COM2), 1);
  outb(SERIAL_INTERRUPT_ENABLE_PORT(COM3), 1);
  outb(SERIAL_INTERRUPT_ENABLE_PORT(COM4), 1);

  serial_configure_baud_rate(COM1, 3); // Set divisor
  serial_configure_baud_rate(COM2, 3);
  serial_configure_baud_rate(COM3, 3);
  serial_configure_baud_rate(COM4, 3);
}