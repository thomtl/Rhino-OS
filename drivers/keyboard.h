#ifndef KEYBOARD_H
#define KEYBOARD_H
#include "../kernel/arch/x86/type.h"

#define KBD_REG_DATA 0x60
#define KBD_REG_COMMAND 0x64

#define KBD_REG_INT_CONT_SELF_TEST 0xAA
#define KBD_REG_INT_DEV_1_SELF_TEST 0xAB
#define KBD_REG_INT_DEV_2_SELF_TEST 0xA9
#define KBD_REG_INT_READ_CONF_BYTE 0x20
#define KBD_REG_INT_WRITE_CONF_BYTE 0x60
#define KBD_REG_INT_DISABLE_DEV_1 0xAD
#define KBD_REG_INT_ENABLE_DEV_1 0xAE
#define KBD_REG_INT_DISABLE_DEV_2 0xA7
#define KBD_REG_INT_ENABLE_DEV_2 0xA8
#define KBD_BUF_FLUSH_LEN 25
uint8_t kbd_read(uint8_t reg);
void kbd_write(uint8_t reg, uint8_t data);
void init_keyboard();
char kbd_getchar();
#endif
