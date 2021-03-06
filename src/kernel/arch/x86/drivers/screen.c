#include <rhino/arch/x86/drivers/screen.h>
#include <rhino/arch/x86/io.h>
#include <libk/string.h>
#include <rhino/Deer/deer.h>
#include <stdint.h>
// private helping funtions
int get_cursor_offset();
void set_cursor_offset(int offset);
int print_char(char c, int col, int row, char attr);
int get_offset(int col, int row);
int get_offset_row(int offset);
int get_offset_col(int offset);

// Global vars
uint8_t color = WHITE_ON_BLACK;
bool use_deer = false;
// Public Kernel API

/*
 * Print a mssage on the specified location
 * if col and row are negative we will use the current offset
**/
void set_color(enum vga_color fg, enum vga_color bg){
  color = fg | bg << 4;
}

uint8_t get_color(){
  return color;
}
void set_raw_color(uint8_t col){
  color = col;
}

void kprint_at(char *message, int col, int row, uint8_t backup){
  int offset;
  int b;
  if(backup == 1){
    b = get_cursor_offset();
  }
  if(col >= 0 && row >= 0){
    offset = get_offset(col, row);
  } else {
    offset = get_cursor_offset();
    row = get_offset_row(offset);
    col = get_offset_col(offset);
  }
  int i = 0;
  while(message[i] != 0){
    offset = print_char(message[i++], col, row, color);
    row = get_offset_row(offset);
    col = get_offset_col(offset);
  }
  if(backup == 1){
    set_cursor_offset(b);
  }
}

void kprint(char *message){
  kprint_at(message, -1, -1, 0);
}
void kprint_err(char *message){
  uint8_t tmp = color;
  set_color(VGA_COLOR_RED, VGA_COLOR_BLACK);
  kprint_at(message, -1, -1, 0);
  color = tmp;
}
void kprint_warn(char *message){
  uint8_t tmp = color;
  set_color(VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
  kprint_at(message, -1, -1, 0);
  color = tmp;
}
void clear_screen(){
  if(!use_deer){
    int screen_size = MAX_COLS * MAX_ROWS;
    int i;
    char *screen = VIDEO_ADDRESS;
    for(i = 0; i < screen_size; i++){
      screen[i*2] = ' ';
      screen[i*2+1] = WHITE_ON_BLACK;
    }
    set_cursor_offset(get_offset(0,0));
  } else {
    deer_clear_screen();
  }
}
void kprint_backspace(){
  int offset = get_cursor_offset() - 2;
  int row = get_offset_row(offset);
  int col = get_offset_col(offset);
  print_char(0x08, col, row, color);

}
// Private Kernel functions

// Innermost print function prints a char


int print_char(char c, int col, int row, char attr){
  if(!use_deer){
    char *vidmem = VIDEO_ADDRESS;
    if(!attr) attr = WHITE_ON_BLACK;

    if(col >= MAX_COLS || row >= MAX_ROWS){
      vidmem[2*(MAX_COLS)*(MAX_ROWS)-2] = 'E';
      vidmem[2*(MAX_COLS)*(MAX_ROWS)-1] = RED_ON_WHITE;
      return get_offset(col, row);
    }
    int offset;
    if(col >= 0 || row >= 0) offset = get_offset(col, row);
    else offset = get_cursor_offset();

    if(c == '\n'){
      row = get_offset_row(offset);
      offset = get_offset(0, row+1);
    } else if(c == 0x08){
      vidmem[offset] = ' ';
      vidmem[offset+1] = attr;
    } else {
      vidmem[offset] = c;
      vidmem[offset+1] = attr;
      offset += 2;
    }

    if(offset >= MAX_ROWS * MAX_COLS * 2){
      for(int i = 1; i < MAX_ROWS; i++){
        memcpy((uint8_t*)(get_offset(0,i-1) + VIDEO_ADDRESS), (uint8_t*)(get_offset(0,i) + VIDEO_ADDRESS), (size_t)(MAX_COLS * 2));
      }
      char *last_line = (char*)(get_offset(0, MAX_ROWS - 1) + VIDEO_ADDRESS);
      for(int i = 0; i < MAX_COLS * 2; i++) last_line[i] = 0;

      offset -= 2 * MAX_COLS;
    }
    set_cursor_offset(offset);
    return offset;
  } else {
    char r, b, g;
    if((color & 0xF) == 0){
      r = g = b = 0;
    } else if((color & 0xF) == 1){
      r = g = 0;
      b = 0xAA;
    } else if((color & 0xF) == 2){
      r = b = 0;
      g = 0xAA;
    } else if((color & 0xF) == 3){
      r = 0;
      g = b = 0xAA;
    } else if((color & 0xF) == 4){
      g = b = 0;
      r = 0xAA;
    } else if((color & 0xF) == 5){
      r = b = 0xAA;
      g = 0;
    } else if((color & 0xF) == 6){
      r =  0xAA;
      g = 0x55;
      b = 0;
    } else if((color & 0xF) == 7){
      r = g = b = 0xAA;
    } else if((color & 0xF) == 8){
      r = g = b = 0x55;
    } else if((color & 0xF) == 9){
      r = g = 0x55;
      b = 0xFF;
    } else if((color & 0xF) == 10){
      r = b = 0x55;
      g = 0xFF;
    } else if((color & 0xF) == 11){
      g = b = 0xFF;
      r = 0x55;
    } else if((color & 0xF) == 12){
      g = b = 0x55;
      r = 0xFF;
    } else if((color & 0xF) == 13){
      r = b = 0xFF;
      g = 0x55;
    } else if((color & 0xF) == 14){
      r = g = 0xFF;
      b = 0x55;
    } else if((color & 0xF) == 15){
      r = g = b = 0xFF;
    } else {
      r = g = b = 0x00;
    }
    deer_putchar(c, r, g, b);
    return 0;
  }
  return 0;
}

int get_cursor_offset(){
  outb(REG_SCREEN_CTRL, 14);
  int offset = inb(REG_SCREEN_DATA) << 8;
  outb(REG_SCREEN_CTRL, 15);
  offset += inb(REG_SCREEN_DATA);
  return offset * 2;
}

void set_cursor_offset(int offset){
  offset /= 2;
  outb(REG_SCREEN_CTRL, 14);
  outb(REG_SCREEN_DATA, (unsigned char)(offset >> 8));
  outb(REG_SCREEN_CTRL, 15);
  outb(REG_SCREEN_DATA, (unsigned char)(offset & 0xff));
}
int get_offset(int col, int row) { return 2 * (row * MAX_COLS + col); }
int get_offset_row(int offset) { return offset / (2 * MAX_COLS); }
int get_offset_col(int offset) { return (offset - (get_offset_row(offset)*2*MAX_COLS))/2; }

void activate_deer(){
  use_deer = true;
}