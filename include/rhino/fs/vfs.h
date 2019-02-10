#pragma once

#include <rhino/common.h>
#include <rhino/types/binary_tree.h>
#include <rhino/types/linked_list.h>
#include <rhino/multitasking/spinlock.h>

#include <libk/string.h>

#define MAX_PATH 1024
#define MAX_FILENAME_LEN 128

#define FS_FILE        0x01
#define FS_DIRECTORY   0x02
#define FS_CHARDEVICE  0x03
#define FS_BLOCKDEVICE 0x04
#define FS_PIPE        0x05
#define FS_SYMLINK     0x06
#define FS_MOUNTPOINT  0x08

struct fs_node;

struct dirent{
  char name[MAX_FILENAME_LEN];
  uint32_t inode;
};

typedef uint32_t (*read_type_t)(struct fs_node*,uint32_t,uint32_t,uint8_t*);
typedef uint32_t (*write_type_t)(struct fs_node*,uint32_t,uint32_t,uint8_t*);
typedef void (*open_type_t)(struct fs_node*);
typedef void (*close_type_t)(struct fs_node*);
typedef struct dirent * (*readdir_type_t)(struct fs_node*,uint32_t);
typedef struct fs_node * (*finddir_type_t)(struct fs_node*,char *name);

typedef struct fs_node {
  char name[MAX_FILENAME_LEN];
  uint32_t mask;
  uint32_t uid;
  uint32_t gid;
  uint32_t inode;
  uint64_t flags;
  uint64_t length;
  uint32_t impl;
  uint32_t refcount;
  read_type_t read;
  write_type_t write;
  open_type_t open;
  close_type_t close;
  readdir_type_t readdir;
  finddir_type_t finddir;
  struct fs_node *ptr;
} fs_node_t;

struct vfs_entry {
  char* name;
  fs_node_t* file;
  char* device;
  char* fs_type;
};

extern fs_node_t *fs_root;

uint32_t read_fs(fs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer);
uint32_t write_fs(fs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer);
void open_fs(fs_node_t *node, uint8_t read, uint8_t write);
void close_fs(fs_node_t *node);
struct dirent *readdir_fs(fs_node_t *node, uint32_t index);
fs_node_t *finddir_fs(fs_node_t *node, char *name);
void* vfs_mount(char* path, fs_node_t* local_root);
void init_vfs();


#define FNM_NOCASE (1 << 0)
bool fnmatch(const char* wild, const char* tame, const uint32_t flags);

/*#define FOPEN 0
#define FCLOSE 1
#define FREAD 2
#define FWRITE 3

#define FILE_SYS_SUCCESS true
#define FILE_SYS_FAILURE false
typedef bool (*file_sys_p)(uint32_t, uint64_t, uint32_t, uint64_t, void*, void*);

struct DPB {
  uint8_t valid;
  uint8_t ldrv;
  char name[32];
  uint8_t active;
  uint32_t access;
  uint32_t dev;
  file_sys_p filesys;
  uint8_t fs_type;
  uint64_t base_lba;
  uint64_t size;
  uint64_t freespace;
  uint16_t sect_size;
  uint32_t info;
  char cur_path[MAX_PATH];
  char reserved[3662];
}; // Drive Parameter Block

struct FILE {
  bool valid;
  uint8_t reserved0[3];
  struct DPB* dpb;
  uint32_t access;
  uint64_t f_pos;
  uint64_t f_size;
  uint64_t prev_lba;
  uint64_t parent;
  uint64_t strtclst;
  uint32_t root_index;
  uint32_t dword;
  uint32_t reserved1;
};

struct DEV {
  uint8_t identifier;
  file_sys_p file_sys;
};

struct FILE_FIND {
  uint8_t valid;
  uint8_t ldrive;
  char search[MAX_FILENAME_LEN + 1];
  uint32_t sattrbs;
  uint32_t attribute;
  struct {
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
  } time;

  struct {
    uint16_t year;
    uint8_t month;
    uint8_t day;
  } date;

  uint64_t fsize;
  char filename[MAX_FILENAME_LEN+1];
  uint8_t ftype_data[32];
};

struct FILE* fopen(const char* path, uint32_t attrib);
bool fclose(struct FILE* fp);

bool fread(void* buf, const uint32_t count, struct FILE* fp);
bool fwrite(void* buf, const uint32_t count, struct FILE* fp);

bool mount(struct DEV *device, uint64_t base_lba);
bool umount(struct DPB* dpb);

bool mkdir(const char* dir);
bool rmdir(const char* dir);

bool get_current_dir(char *dir);
bool set_current_dir(const char* dir);

bool file_find(struct FILE_FIND* find);
bool find_next(struct FILE_FIND* find);

struct DPB* get_dpb_log_drive(const char logdrv);
struct DPB* get_dpb_name(const char* name);
struct DPB* get_current_dpb();
bool set_current_dpb(struct DPB* dpb);

bool fset_attrb(const char* file, const uint32_t attr);
uint32_t fget_attrb(const char* file);
bool get_vol_serial(const struct DPB *dpb, struct GUID* guid);

uint64_t free_space(const struct DPB* dpb);

bool truename(const char* file, char *truename);





// STDIO Part

struct FILE* stdio_fopen(const char* path, uint32_t attrib);
bool stdio_fclose(struct FILE* fp);

bool stdio_fread(void* buf, const uint32_t count, struct FILE* fp);
bool stdio_fwrite(void* buf, const uint32_t count, struct FILE* fp);
bool fseek(const uint64_t pos, const int from, struct FILE* fp);

bool frewind(struct FILE* fp);
bool fremove(const char* name);

uint64_t ftell(const struct FILE* fp);
uint64_t fgetpos(const struct FILE* fp);*/
