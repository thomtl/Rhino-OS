#include <libk/stdio.h>
#include <rhino/arch/x86/drivers/keyboard.h>
char getchar(){
    return kbd_getchar();
}

char* getline(char lineptr[], size_t bufsize){
  //char str[256] = "";
  for(uint16_t i = 0; i < bufsize; i++){
    lineptr[i] = '\0';
  }
  char c;
  uint32_t pos = 0;
  while(1){
    c = getchar();
    if(c == '\b'){
      lineptr[pos - 1] = ' ';
      pos -= 2;
    } else if(c == '\n'){
      lineptr[pos] = '\0';
      return lineptr;
    } else {
      lineptr[pos] = c;
    }
    pos++;
  }

}