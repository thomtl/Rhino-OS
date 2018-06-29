#ifndef KEYBOARD_H
#define KEYBOARD_H
#include "../kernel/arch/x86/type.h"
void init_keyboard();
char kbd_getchar();
#endif
