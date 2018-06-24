#include "program.h"
#include "../../drivers/screen.h"
#include "../fs/vfs.h"
#include "../fs/initrd.h"
#include "../heap/kheap.h"
#include "../../libc/function.h"
#include "../../libc/include/string.h"


/**
   @brief Loads a program from the INITRD into Memory.
   @param args List of args.  args[0] is the filename of the file to search.  args[1] is the binary type of the file.
   @return returns a pointer to a struct which contains info over the loaded program, in case of an error it will return 0.
 */
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
    if(type == PROGRAM_BINARY_TYPE_BIN){
      fs_node_t *node = 0;
      node = finddir_fs(fs_root, filename);
      if(node == 0){
        kprint("program.c: Could not find program on the INITRD\n");
        return 0;
      }
      if((node->flags & 0x7) == FS_DIRECTORY){
        kprint("program.c: Selected File is a Directory\n");
        return 0;
      }
      loaded_program_t* header = kmalloc(sizeof(loaded_program_t));
      uint32_t* buf = kmalloc(node->length);
      header->base = (uint32_t*)buf;
      header->end = (uint32_t*)(buf + node->length);
      header->type = type;
      read_fs(node, 0, node->length, (uint8_t*)buf);
      return header;
    } else {
      kprint("program.c: ELF and A.OUT types are not yet implemented!\n");
      return 0;
    }
}
/**
   @brief Calls a program in memory.
   @param args List of args.  args[0] is the address to call.
   @return returns nothing.
 */
void run_program(void *address){
  call_module_t prog = (call_module_t) address;
  prog();
  return;
}

/**
   @brief Frees a program in memory.
   @param args List of args.  args[0] is the address of the program info struct.
   @return returns nothing.
 */
void free_program(loaded_program_t* header){
  kfree(header->base);
  kfree(header);
}
