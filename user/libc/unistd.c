#include "include/unistd.h"

int chdir(const char* path){
    return syscall(11, (uint32_t)path, syscall(3, 0, 0, 0), 0);
}

char* getcwd(char* buf, size_t size){
    syscall(12, (uint32_t)buf, size, 0);
    return buf;
}