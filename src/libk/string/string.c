#include <libk/string.h>

char * strcpy(char *strDest, char *strSrc)
{
    char *temp = strDest;
    while((*strDest++ = *strSrc++)); // or while((*strDest++=*strSrc++) != '\0');
    return temp;
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
void hex_to_ascii(int n, char str[]){
  append(str, '0');
  append(str, 'x');

  char zeros = 0;
  int32_t tmp;
  for(int i = 28; i > 0; i -= 4){
    tmp = (n >> i) & 0xF;
    if(tmp == 0 && zeros == 0) continue;
    zeros -= 1;
    if(tmp >= 0xA) append(str, tmp - 0xA + 'a');
    else append(str, tmp + '0');
  }
  tmp = n & 0xF;
  if(tmp >= 0xA) append(str, tmp - 0xA + 'a');
  else append(str, tmp + '0');

}
void reverse(char s[]){
  int c;
  for(int i = 0, j = (int)(strlen(s)-1); i < j; i++, j--){
    c = s[i];
    s[i] = s[j];
    s[j] = c;
  }
}

void backspace(char s[]){
  int len = strlen(s);
  s[len-1] = '\0';
}
void append(char s[], char n){
  int len = strlen(s);
  s[len] = n;
  s[len+1] = '\0';
}
int strcmp(char s1[], char s2[]){
  int i;
  for(i = 0; s1[i] == s2[i]; i++){
    if(s1[i] == '\0') return 0;
  }
  return s1[i] - s2[i];
}

char* strdup(char* s){
  size_t len = strlen(s) + 1;
  char* d = malloc(len);
  if(d == NULL) return NULL;
  memset(d, 0, len);
  strcpy(d, s);
  return d;
}