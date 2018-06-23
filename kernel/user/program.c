#include "program.h"
#include "../../drivers/screen.h"
#include "../fs/vfs.h"
#include "../fs/initrd.h"
#include "../heap/kheap.h"
#include "../../libc/function.h"
#include "../../libc/include/string.h"
loaded_program_t* load_program(char* filename, uint8_t type){
    /*int i = 0;
    struct dirent *node = 0;
    while ( (node = readdir_fs(fs_root, i)) != 0)
    {
      //kprint(node->name);
      if(strcmp(node->name, filename) != 0) continue; 
      fs_node_t *fsnode = finddir_fs(fs_root, node->name);

      if ((fsnode->flags&0x7) == FS_DIRECTORY){
        kprint("program.c: selected file is a directory");
      } else {
        //kprint("\n\t contents: \"");
        //uint8_t buf[256];
        loaded_program_t* header = kmalloc(sizeof(loaded_program_t));
        uint8_t* buf = kmalloc(fsnode->length);
        header->base = (uint32_t*)buf;
        header->end = (uint32_t*)(buf + fsnode->length);
        header->type = type;
        read_fs(fsnode, 0, fsnode->length, buf);
        return header;
      }
      i++;
    }
    kprint("program.c: ERROR Could not find chosen file");
    return NULL;*/
    fs_node_t *node = 0;
    node = finddir_fs(fs_root, filename);
    if(node == 0){
      kprint("program.c: Could not find program on the INITRD\n");
      return 0;
    }
    loaded_program_t* header = kmalloc(sizeof(loaded_program_t));
    uint8_t* buf = kmalloc(node->length);
    header->base = (uint32_t*)buf;
    header->end = (uint32_t*)(buf + node->length);
    header->type = type;
    read_fs(node, 0, node->length, buf);
    return header;
}