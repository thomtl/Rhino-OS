#include "keyboard.h"
#include "../kernel/arch/x86/ports.h"
#include "../kernel/arch/x86/isr.h"
#include "screen.h"
#include "../libc/include/string.h"
#include "../kernel/common.h"
#include "../kernel/kernel.h"

#define BACKSPACE 0x0E
#define ENTER 0x1C

#define MAX_STDIN_LENGTH 256
static char stdin[MAX_STDIN_LENGTH];
static uint8_t stdinIndex = 0;
static uint8_t readIndex = 0;
#define SC_MAX 57

const char *sc_name[] = { "ERROR", "Esc", "1", "2", "3", "4", "5", "6",
    "7", "8", "9", "0", "-", "=", "Backspace", "Tab", "Q", "W", "E",
        "R", "T", "Y", "U", "I", "O", "P", "[", "]", "Enter", "Lctrl",
        "A", "S", "D", "F", "G", "H", "J", "K", "L", ";", "'", "`",
        "LShift", "\\", "Z", "X", "C", "V", "B", "N", "M", ",", ".",
        "/", "RShift", "Keypad *", "LAlt", "Spacebar"};
const char sc_ascii[] = { '?', '?', '1', '2', '3', '4', '5', '6',
    '7', '8', '9', '0', '-', '=', '?', '?', 'Q', 'W', 'E', 'R', 'T', 'Y',
        'U', 'I', 'O', 'P', '[', ']', '?', '?', 'A', 'S', 'D', 'F', 'G',
        'H', 'J', 'K', 'L', ';', '\'', '`', '?', '\\', 'Z', 'X', 'C', 'V',
'B', 'N', 'M', ',', '.', '/', '?', '?', '?', ' '};

static void kbd_irq(registers_t *regs){
  uint8_t scancode;
  if(BIT_IS_SET(inb(0x64), 0)){
    scancode = inb(0x60);
  } else {
    kprint_err("Keyboard sent IRQ but there was no data\n");
    return;
  }
  if(scancode > SC_MAX) return;
  if(scancode == ENTER){
    kprint("\n");
    append(stdin, '\n');
    stdinIndex++;
  } else if (scancode == BACKSPACE){
    //backspace(stdin);
    //stdinIndex--;
    //readIndex--;
    kprint_backspace();
    append(stdin, '\b');
    stdinIndex++;
  } else {
    append(stdin, sc_ascii[(int)scancode]);
    char str[2] = {stdin[stdinIndex], '\0'};
    stdinIndex++;
    kprint(str);
  }
  UNUSED(regs);
}


void init_keyboard(){
  register_interrupt_handler(IRQ1, kbd_irq);
}

char kbd_getchar(){
  while(stdinIndex <= readIndex);
  char ascii = stdin[readIndex];
  readIndex++;
  return ascii;
}
