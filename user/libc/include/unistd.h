#ifndef RHINO_LIBC_UNISTD_H
#define RHINO_LIBC_UNISTD_H

#include "cdefs.h"
#include <stddef.h>
#include "sys.h"

int chdir(const char* path);
char* getcwd(char* buf, size_t size);

#endif