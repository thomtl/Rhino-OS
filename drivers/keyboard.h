#ifndef KEYBOARD_H
#define KEYBOARD_H
#include "../kernel/arch/x86/type.h"

#define KBD_REG_DATA 0x60
#define KBD_REG_COMMAND 0x64

#define KBD_REG_INT_CONT_SELF_TEST 0xAA
#define KBD_REG_INT_KEYB_SELF_TEST 0xAB
#define KBD_REG_INT_READ_CONF_BYTE 0x20
#define KBD_REG_INT_WRITE_CONF_BYTE 0x60

void init_keyboard();
char kbd_getchar();
void kbd_test();
#endif
