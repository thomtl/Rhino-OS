#include "include/string.h"

int strcmp(char s1[], char s2[]){
  int i;
  for(i = 0; s1[i] == s2[i]; i++){
    if(s1[i] == '\0') return 0;
  }
  return s1[i] - s2[i];
}

 

size_t strlen(const char* str) {
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}


void reverse(char s[]){
  int c;
  for(int i = 0, j = (int)(strlen(s)-1); i < j; i++, j--){
    c = s[i];
    s[i] = s[j];
    s[j] = c;
  }
}

void int_to_ascii(int n, char str[]){
  int i, sign;
  if((sign = n) < 0) n = -n;
  i = 0;
  do {
    str[i++] = n % 10 + '0';
  } while((n /= 10) > 0);
  if(sign < 0) str[i++] = '-';
  str[i] = '\0';
  reverse(str);
}