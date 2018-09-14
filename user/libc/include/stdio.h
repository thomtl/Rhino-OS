#ifndef RHINO_LIBC_STDIO_H
#define RHINO_LIBC_STDIO_H
#include "cdefs.h"
#include <stdint.h>
#include <stddef.h>
uint8_t putchar(uint8_t c);
void printf(char* msg);
char getchar();
char* getline(char *lineptr, size_t bufsize);

void clear_screen();

#define RHINO_LIBC_FS_FILE        0x01
#define RHINO_LIBC_FS_DIRECTORY   0x02
#define RHINO_LIBC_FS_CHARDEVICE  0x03
#define RHINO_LIBC_FS_BLOCKDEVICE 0x04
#define RHINO_LIBC_FS_PIPE        0x05
#define RHINO_LIBC_FS_SYMLINK     0x06
#define RHINO_LIBC_FS_MOUNTPOINT  0x08

struct dirent {
  char name[128];
  uint32_t ino;
};
struct file_node;
typedef uint32_t (*read_type_t)(struct file_node*,uint32_t,uint32_t,uint8_t*);
typedef uint32_t (*write_type_t)(struct file_node*,uint32_t,uint32_t,uint8_t*);
typedef void (*open_type_t)(struct file_node*);
typedef void (*close_type_t)(struct file_node*);
typedef struct dirent * (*readdir_type_t)(struct file_node*,uint32_t);
typedef struct file_node * (*finddir_type_t)(struct file_node*,char *name);

typedef struct file_node {
  char name[128];
  uint32_t mask;
  uint32_t uid;
  uint32_t gid;
  uint32_t inode;
  uint32_t flags;
  uint32_t length;
  uint32_t impl;
  read_type_t read;
  write_type_t write;
  open_type_t open;
  close_type_t close;
  readdir_type_t readdir;
  finddir_type_t finddir;
  struct file_node *ptr;
} FILE;

FILE* fopen(char* filename);
void fread(void* ptr, size_t size, FILE* file);
#endif