#pragma once

#include <stdint.h>
#include <stddef.h>
#include "../sys.h"

#define	_IFMT		0170000
#define		_IFDIR	0040000
#define		_IFCHR	0020000
#define		_IFBLK	0060000
#define		_IFREG	0100000
#define		_IFLNK	0120000
#define		_IFSOCK	0140000
#define		_IFIFO	0010000

#define	S_ISBLK(m)	(((m)&_IFMT) == _IFBLK)
#define	S_ISCHR(m)	(((m)&_IFMT) == _IFCHR)
#define	S_ISDIR(m)	(((m)&_IFMT) == _IFDIR)
#define	S_ISFIFO(m)	(((m)&_IFMT) == _IFIFO)
#define	S_ISREG(m)	(((m)&_IFMT) == _IFREG)
#define	S_ISLNK(m)	(((m)&_IFMT) == _IFLNK)
#define	S_ISSOCK(m)	(((m)&_IFMT) == _IFSOCK)

typedef int mode_t;

struct stat  {
	uint16_t  st_dev;
	uint16_t  st_ino;
	uint32_t  st_mode;
	uint16_t  st_nlink;
	uint16_t  st_uid;
	uint16_t  st_gid;
	uint16_t  st_rdev;
	int32_t  st_size;
	uint32_t  st_atime;
	uint32_t  unused1;
	int32_t  st_mtime;
	uint32_t  unused2;
	uint32_t  st_ctime;
	uint32_t  unused3;
	uint32_t  st_blksize;
	uint32_t  st_blocks;
};

struct dirent {
  char name[128];
  uint32_t ino;
};

int fstat(int fd, struct stat *st);

int open(char* filename);
void close(int file);
struct dirent* readdir(int fd, uint32_t index);
void read(void* ptr, size_t size, int file);