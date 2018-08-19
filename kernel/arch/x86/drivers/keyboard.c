#include "keyboard.h"
#include "../ports.h"
#include "../isr.h"
#include "../timer.h"
#include "screen.h"
#include "../../../../libk/include/string.h"
#include "../../../common.h"
#include "../../../kernel.h"

#define BACKSPACE 0x0E
#define ENTER 0x1C
#define LSHIFT 0x2A
#define CAPS_LOCK 0x3A
#define MAX_STDIN_LENGTH 256
static char stdin[MAX_STDIN_LENGTH];
static uint32_t stdinIndex = 0;
static uint32_t readIndex = 0;
static bool hasSecondChannel = true;
static bool shiftPressed = false;
#define SC_MAX 57

const char *sc_name[] = { "ERROR", "Esc", "1", "2", "3", "4", "5", "6",
    "7", "8", "9", "0", "-", "=", "Backspace", "Tab", "Q", "W", "E",
        "R", "T", "Y", "U", "I", "O", "P", "[", "]", "Enter", "Lctrl",
        "A", "S", "D", "F", "G", "H", "J", "K", "L", ";", "'", "`",
        "LShift", "\\", "Z", "X", "C", "V", "B", "N", "M", ",", ".",
        "/", "RShift", "Keypad *", "LAlt", "Spacebar"};
const char sc_ascii[] = { '?', '?', '1', '2', '3', '4', '5', '6',
    '7', '8', '9', '0', '-', '=', '?', '?', 'q', 'w', 'e', 'r', 't', 'y',
        'u', 'i', 'o', 'p', '[', ']', '?', '?', 'a', 's', 'd', 'f', 'g',
        'h', 'j', 'k', 'l', ';', '\'', '`', '?', '\\', 'z', 'x', 'c', 'v',
'b', 'n', 'm', ',', '.', '/', '?', '?', '?', ' '};

static void kbd_irq(registers_t *regs){
  uint8_t scancode;
  if(BIT_IS_SET(inb(KBD_REG_COMMAND), 0)){
    scancode = inb(KBD_REG_DATA);
  } else {
    kprint_err("Keyboard sent IRQ but there was no data\n");
    return;
  }
  if(scancode == CAPS_LOCK){
    shiftPressed = !shiftPressed;
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
    char ascii = sc_ascii[(int)scancode];
    if(shiftPressed) ascii -= 32;
    append(stdin, ascii);
    char str[2] = {stdin[stdinIndex], '\0'};
    stdinIndex++;
    kprint(str);
  }
  UNUSED(regs);
}

static void wait_for_res(){
  while(!BIT_IS_SET(inb(KBD_REG_COMMAND), 0));
}

void init_keyboard(){
  uint8_t res, conf;
  // Stop interrupts
  CLI();

  // Disable the 2 devices, if the second channel doesn't exist it will ignore it
  outb(KBD_REG_COMMAND, KBD_REG_INT_DISABLE_DEV_1);
  outb(KBD_REG_COMMAND, KBD_REG_INT_DISABLE_DEV_2);
  // Flush the buffer
  for(uint8_t i = 0; i < KBD_BUF_FLUSH_LEN; i++) inb(KBD_REG_DATA);
  // Init the controller configuration byte
  conf = kbd_read(KBD_REG_INT_READ_CONF_BYTE);
  BIT_CLEAR(conf, 0);
  BIT_CLEAR(conf, 1);
  BIT_CLEAR(conf, 6);
  if(!BIT_IS_SET(conf, 5)) hasSecondChannel = false;
  kbd_write(KBD_REG_INT_WRITE_CONF_BYTE, conf);

  // Perform controller self-test
  outb(KBD_REG_COMMAND,  KBD_REG_INT_CONT_SELF_TEST);
  wait_for_res();
  res = inb(KBD_REG_DATA);
  if(res != 0x55) PANIC_M("8042 Controller did not successfully self-test");

  // Determine if there are 2 channels
  if(hasSecondChannel == true){
    // Temporarly enable the second channel
    outb(KBD_REG_COMMAND, KBD_REG_INT_ENABLE_DEV_2);

    // Read controller configuration again
    conf = kbd_read(KBD_REG_INT_READ_CONF_BYTE);
    if(!BIT_IS_SET(conf, 5)){
      hasSecondChannel = true;
    } else if (BIT_IS_SET(conf, 5)){
      hasSecondChannel = false;
    }

    // Disable channel 2 again
    outb(KBD_REG_COMMAND, KBD_REG_INT_DISABLE_DEV_2);
  }

  // Test the channels themselves
  outb(KBD_REG_COMMAND, KBD_REG_INT_DEV_1_SELF_TEST);
  wait_for_res();
  res = inb(KBD_REG_DATA);
  if(res == 0x01) PANIC_M("PS/2 Channel 1 Clock line stuck low");
  if(res == 0x02) PANIC_M("PS/2 Channel 1 Clock line stuck high");
  if(res == 0x03) PANIC_M("PS/2 Channel 1 Data line stuck low");
  if(res == 0x04) PANIC_M("PS/2 Channel 1 Data line stuck high");

  if(hasSecondChannel == true){
    outb(KBD_REG_COMMAND, KBD_REG_INT_DEV_2_SELF_TEST);
    wait_for_res();
    res = inb(KBD_REG_DATA);
    if(res == 0x01) PANIC_M("PS/2 Channel 2 Clock line stuck low");
    if(res == 0x02) PANIC_M("PS/2 Channel 2 Clock line stuck high");
    if(res == 0x03) PANIC_M("PS/2 Channel 2 Data line stuck low");
    if(res == 0x04) PANIC_M("PS/2 Channel 2 Data line stuck high");
  }

  // Enable the devices
  outb(KBD_REG_COMMAND, KBD_REG_INT_ENABLE_DEV_1);
  if(hasSecondChannel == true) outb(KBD_REG_COMMAND, KBD_REG_INT_ENABLE_DEV_2);


  // Register the interrupt handler for the first channel
  register_interrupt_handler(IRQ1, kbd_irq);

  /*if (kbd_ch_1_write(0xFF) == 0xFC) PANIC_M("PS/2 Channel 1 Device Reset did not report success");

  if(hasSecondChannel == true){

    if (kbd_ch_2_write(0xFF) == 0xFC) PANIC_M("PS/2 Channel 2 Device Reset did not report success");
  }*/

  conf = kbd_read(KBD_REG_INT_READ_CONF_BYTE);
  BIT_SET(conf, 0);
  BIT_SET(conf, 1);
  BIT_SET(conf, 6);
  kbd_write(KBD_REG_INT_WRITE_CONF_BYTE, conf);

  // Restart interrupts
  STI();
}

char kbd_getchar(){
  while(stdinIndex <= readIndex);
  char ascii = stdin[readIndex];
  readIndex++;
  return ascii;
}
char sys_kbd_getchar(){
  STI();
  while(stdinIndex <= readIndex);
  char ascii = stdin[readIndex];
  readIndex++;
  CLI();
  return ascii;
}

uint8_t kbd_read(uint8_t reg){
  outb(KBD_REG_COMMAND, reg);
  wait_for_res();
  return inb(KBD_REG_DATA);
}

void kbd_write(uint8_t reg, uint8_t data){
  outb(KBD_REG_COMMAND, reg);
  outb(KBD_REG_DATA, data);
}

uint32_t kbd_ch_1_write(uint32_t data){
  while(BIT_IS_SET(inb(KBD_REG_COMMAND), 1));
  outb(KBD_REG_DATA, data);
  return inb(KBD_REG_DATA);
}
uint32_t kbd_ch_2_write(uint32_t data){
  outb(KBD_REG_COMMAND, 0xD4);
  while(BIT_IS_SET(inb(KBD_REG_COMMAND), 1));
  outb(KBD_REG_DATA, data);
  return inb(KBD_REG_DATA);
}
