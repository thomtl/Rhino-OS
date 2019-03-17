#pragma once

#include <rhino/common.h>

typedef uint16_t Elf32_Half;
typedef uint32_t Elf32_Off;
typedef uint32_t Elf32_Addr;
typedef uint32_t Elf32_Word;
typedef int32_t Elf32_Sword;

#define ELF_NIDENT 16

typedef struct
{
    uint8_t e_ident[ELF_NIDENT];
    Elf32_Half e_type;
    Elf32_Half e_machine;
    Elf32_Word e_version;
    Elf32_Addr e_entry;
    Elf32_Off e_phoff;
    Elf32_Off e_shoff;
    Elf32_Word e_flags;
    Elf32_Half e_ehsize;
    Elf32_Half e_phentsize;
    Elf32_Half e_phnum;
    Elf32_Half e_shentsize;
    Elf32_Half e_shnum;
    Elf32_Half e_shstrndx;
} __attribute__ ((packed)) Elf32_Ehdr;

#define ELF_IDENT_MAGIC_0 0
#define ELF_IDENT_MAGIC_1 1
#define ELF_IDENT_MAGIC_2 2
#define ELF_IDENT_MAGIC_3 3
#define ELF_IDENT_CLASS 4
#define ELF_IDENT_DATA 5
#define ELF_IDENT_VERSION 6
#define ELF_IDENT_OSABI 7
#define ELF_IDENT_ABIVERSION 8
#define ELF_IDENT_PAD 9

#define ELF_MAGIC_0 0x7F
#define ELF_MAGIC_1 'E'
#define ELF_MAGIC_2 'L'
#define ELF_MAGIC_3 'F'

#define ELF_DATA_LSB 1
#define ELF_CLASS_32 1

#define ELF_TYPE_NONE 0
#define ELF_TYPE_RELOCATABLE 1
#define ELF_TYPE_EXECUTABLE 2

#define ELF_MACHINETYPE_386 3
#define ELF_CURRENT_VERSION 1

typedef struct {
    Elf32_Word sh_name;
    Elf32_Word sh_type;
    Elf32_Word sh_flags;
    Elf32_Addr sh_addr;
    Elf32_Off sh_offset;
    Elf32_Word sh_size;
    Elf32_Word sh_link;
    Elf32_Word sh_info;
    Elf32_Word sh_addralign;
    Elf32_Word sh_entsize;
} __attribute__ ((packed)) Elf32_Shdr;

#define SHN_UNDEF 0x0
#define SHN_ABS 0xFFF1
#define SHN_COMMON 0xFFF2
#define SHN_XINDEX 0xFFFF
#define SHN_LORESERVE 0xFF00

#define SHT_NULL 0
#define SHT_PROGBITS 1
#define SHT_SYMTAB 2
#define SHT_STRTAB 3
#define SHT_RELA 4
#define SHT_NOBITS 8
#define SHT_REL 9

#define SHF_WRITE 0x1
#define SHF_ALLOC 0x2

typedef struct {
    Elf32_Word st_name;
    Elf32_Addr st_value;
    Elf32_Word st_size;
    uint8_t st_info;
    uint8_t st_other;
    Elf32_Half st_shndx;
} __attribute__ ((packed)) Elf32_Sym;

#define ELF32_ST_BIND(info) (((info) >> (4)) & (0xF))
#define ELF32_ST_TYPE(info) ((info) & (0xF))

#define STB_LOCAL 0
#define STB_GLOBAL 1
#define STB_WEAK 2

#define STT_NOTYPE 0
#define STT_OBJECT 1
#define STT_FUNC 2

typedef struct {
    Elf32_Addr r_offset;
    Elf32_Word r_info;
} __attribute__ ((packed)) Elf32_Rel;

typedef struct {
    Elf32_Addr r_offset;
    Elf32_Word r_info;
    Elf32_Sword r_addend;
} __attribute__ ((packed)) Elf32_Rela;

#define ELF32_R_SYM(info) (((info) >> (8)) & 0xFF)
#define ELF32_R_TYPE(info) ((info) & (0xFF))

#define R_386_NONE 0
#define R_386_32 1
#define R_386_PC32 2

#define DO_386_32(s, a) ((s) + (a))
#define DO_386_PC32(s, a, p) ((s) + (a) - (p))

typedef struct {
    Elf32_Word p_type;
    Elf32_Off p_offset;
    Elf32_Addr p_vaddr;
    Elf32_Addr p_paddr;
    Elf32_Word p_filesz;
    Elf32_Word p_memsz;
    Elf32_Word p_flags;
    Elf32_Word p_align;
} __attribute__ ((packed)) Elf32_Phdr;

#define PT_LOAD 1
#define PF_X 1
#define PF_R 2
#define PF_w 4

bool elf_is_elf(void* file);
uint32_t elf_get_load_addr(void* file);
void* elf_load_file(void* file);
