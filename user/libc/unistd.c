#include "include/unistd.h"

int chdir(const char* path){
    syscall(0, 11, (uint32_t)path, syscall(0, 3, 0, 0));
    return 0;
}

char* getcwd(char* buf, size_t size){
    syscall(0, 12, (uint32_t)buf, size);
    return buf;
}