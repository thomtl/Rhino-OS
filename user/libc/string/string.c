#include "../include/string.h"

int strcmp(char s1[], char s2[]){
  int i;
  for(i = 0; s1[i] == s2[i]; i++){
    if(s1[i] == '\0') return 0;
  }
  return s1[i] - s2[i];
}

char * strcpy(char *strDest, char *strSrc)
{
    char *temp = strDest;
    while((*strDest++ = *strSrc++)); // or while((*strDest++=*strSrc++) != '\0');
    return temp;
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

char *strchr(const char *s, int c){
  while(*s != (char)c)
    if(!*s++)
      return 0;
  return (char*)s;
}

size_t strcspn(const char *s1, const char *s2){
  size_t ret = 0;
  while(*s1)
    if(strchr(s2, *s1))
      return ret;
    else
      s1++,ret++;
  return ret;
}

size_t strspn(const char *s1, const char *s2){
  size_t ret = 0;
  while(*s1 && strchr(s2, *s1++))
    ret++;
  return ret;
}

char *strtok(char* str, const char* restrict delim){
  static char* p = 0;
  if(str) p = str;
  else if(!p) return 0;
  str = p + strspn(p, delim);
  p = str + strcspn(str, delim);
  if(p == str) return p = 0;
  p = *p ? *p=0,p+1 : 0;
  return str;
}