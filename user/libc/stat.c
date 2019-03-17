#include "include/sys/stat.h"


#define	_IFMT		0170000	/* type of file */
#define		_IFDIR	0040000	/* directory */
#define		_IFCHR	0020000	/* character special */
#define		_IFBLK	0060000	/* block special */
#define		_IFREG	0100000	/* regular */
#define		_IFLNK	0120000	/* symbolic link */
#define		_IFSOCK	0140000	/* socket */
#define		_IFIFO	0010000	/* fifo */

int fstat(int fd, struct stat *st){
    return syscall(26, fd, (uint32_t)st, 0);
}

int open(char* filename){
  return (int)syscall(18, (uint32_t)filename, 2, 0); // rdwr
}

void close(int file){
  syscall(19, (uint32_t)file, 0, 0);
}

void read(void* ptr, size_t size, int file){
  syscall(21, (uint32_t)file, size, (uint32_t)ptr);
}

struct dirent* readdir(int fd, uint32_t index){
    uint32_t dirent = syscall(20, fd, index, 0);

    return (struct dirent*)dirent;
}