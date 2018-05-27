#include "paging.h"

void starttt(){
  int i = 0;
  for(i = 0; i < 1024; i++){
    page_directory[i] = 0x00000002;
  }
  for(i = 0; i < 1024; i++){
    page_table[i] = (i * 0x1000) | 3;
  }
  page_directory[0] = ((uint32_t)page_table) | 3;
  loadPageDirectory(page_directory);
  enablePaging();
}
