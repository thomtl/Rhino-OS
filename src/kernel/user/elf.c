#include <rhino/user/elf.h>

static bool elf_check_file(Elf32_Ehdr* hdr){
    if(!hdr) return false;

    if(hdr->e_ident[ELF_IDENT_MAGIC_0] != ELF_MAGIC_0){
        debug_log("[ELF]: hdr->e_ident magic 0 is not correct\n");
        return false;
    }

    if(hdr->e_ident[ELF_IDENT_MAGIC_1] != ELF_MAGIC_1){
       debug_log("[ELF]: hdr->e_ident magic 1 is not correct\n");
        return false; 
    }

    if(hdr->e_ident[ELF_IDENT_MAGIC_2] != ELF_MAGIC_2){
        debug_log("[ELF]: hdr->e_ident magic 2 is not correct\n");
        return false;
    }
    if(hdr->e_ident[ELF_IDENT_MAGIC_3] != ELF_MAGIC_3){
        debug_log("[ELF]: hdr->e_ident magic 3 is not correct\n");
        return false;
    }

    return true;
}

static bool elf_supported(Elf32_Ehdr* hdr){
    if(!elf_check_file(hdr)){
        debug_log("[ELF]: Invalid ELF File\n");
        return false;
    }

    if(hdr->e_ident[ELF_IDENT_CLASS] != ELF_CLASS_32){
        debug_log("[ELF]: Unsupported ELF File Class\n");
        return false;
    }

    if(hdr->e_ident[ELF_IDENT_DATA] != ELF_DATA_LSB){
        debug_log("[ELF]: Unsupported ELF Byte Order\n");
        return false;
    }

    if(hdr->e_machine != ELF_MACHINETYPE_386){
        debug_log("[ELF]: Unsupported ELF Machine Type\n");
        return false;
    }

    if(hdr->e_ident[ELF_IDENT_VERSION] != ELF_CURRENT_VERSION){
        debug_log("[ELF]: Unsupported ELF Version\n");
        return false;
    }

    if(hdr->e_type != ELF_TYPE_EXECUTABLE && hdr->e_type != ELF_TYPE_RELOCATABLE){
        debug_log("[ELF]: Unsupported ELF File Type\n");
        return false;
    }

    return true;
}




static bool elf_load_segment(Elf32_Ehdr* hdr, Elf32_Phdr* phdr){
    // TODO: Change MM Flags

    if(phdr->p_memsz == 0) return false;

    uint8_t* data = (uint8_t*)((uint32_t)hdr + phdr->p_offset);

    memcpy((void*)phdr->p_vaddr, data, phdr->p_filesz);
    memset((void*)(phdr->p_vaddr + phdr->p_filesz), 0, phdr->p_memsz - phdr->p_filesz);
    return true;
}


static void* elf_load_executable(Elf32_Ehdr* hdr){
    Elf32_Phdr* phdr = (Elf32_Phdr*)((int)hdr + hdr->e_phoff);

    for(uint32_t i = 0; i < hdr->e_phnum; i++){
        if(phdr[i].p_type == PT_LOAD){
            if(!elf_load_segment(hdr, &phdr[i])){
                debug_log("[ELF]: Failed to load segment\n");
            }
        }
    }

    return (void*)hdr->e_entry;
}

bool elf_is_elf(void* file){
    Elf32_Ehdr* hdr = (Elf32_Ehdr*)file;

    for(int i = 0; i < 4; i++){
        char buf[25] = "";
        hex_to_ascii(((uint8_t*)file)[i], buf);
        debug_log(buf);
        debug_log("\n");
    }

    if(hdr->e_ident[ELF_IDENT_MAGIC_0] != ELF_MAGIC_0) return false;
    if(hdr->e_ident[ELF_IDENT_MAGIC_1] != ELF_MAGIC_1) return false; 
    if(hdr->e_ident[ELF_IDENT_MAGIC_2] != ELF_MAGIC_2) return false;
    if(hdr->e_ident[ELF_IDENT_MAGIC_3] != ELF_MAGIC_3) return false;

    return true;
}

uint32_t elf_get_load_addr(void* file){
    Elf32_Ehdr* hdr = (Elf32_Ehdr*)file;
    if(!elf_supported(hdr)){
        debug_log("[ELF]: File invalid or not supported\n");
        return 0;
    }
    Elf32_Phdr* phdr = (Elf32_Phdr*)((int)hdr + hdr->e_phoff);
    return phdr->p_vaddr;
}

uint32_t elf_get_file_length(void* file){
    Elf32_Ehdr* hdr = (Elf32_Ehdr*)file;
    if(!elf_supported(hdr)){
        debug_log("[ELF]: File invalid or not supported\n");
        return 0;
    }
    Elf32_Phdr* phdr = (Elf32_Phdr*)((int)hdr + hdr->e_phoff);
    return phdr->p_memsz;
}



void* elf_load_file(void* file){
    Elf32_Ehdr* hdr = (Elf32_Ehdr*)file;
    if(!elf_supported(hdr)){
        debug_log("[ELF]: File not supported\n");
        return NULL;
    }

    switch (hdr->e_type)
    {
        case ELF_TYPE_EXECUTABLE:
            return elf_load_executable(hdr);
    
        case ELF_TYPE_RELOCATABLE:
            return NULL; //TODO: Half implementd elf_load_rel(hdr);

        default:
            debug_log("[ELF]: Unkown Executable header type\n");
            break;
    }

    return NULL;
}

/*static inline Elf32_Shdr* elf_sheader(Elf32_Ehdr* hdr){
    return (Elf32_Shdr*)((uint32_t)hdr + hdr->e_shoff);
}

static inline Elf32_Shdr* elf_section(Elf32_Ehdr* hdr, int i){
    return &(elf_sheader(hdr)[i]);
}

static inline char* elf_str_table(Elf32_Ehdr* hdr){
    if(hdr->e_shstrndx == SHN_UNDEF) return NULL;

    return (char*)hdr + elf_section(hdr, hdr->e_shstrndx)->sh_offset;
}

static inline char* elf_loopup_string(Elf32_Ehdr* hdr, int offset){
    char* strtab = elf_str_table(hdr);
    if(strtab == NULL) return NULL;

    return strtab + offset;
}

static void* elf_loopup_symbol(const char* name){
    // TODO: Implement
    return NULL;
}

static uint32_t elf_get_symval(Elf32_Ehdr* hdr, uint32_t table, uint32_t i){
    if(table == SHN_UNDEF || i == SHN_UNDEF) return 0;

    Elf32_Shdr* symtab = elf_section(hdr, table);

    uint32_t symtab_entries = symtab->sh_size / symtab->sh_entsize;

    if(i >= symtab_entries){
        debug_log("[ELF]: Symbol index out of range\n");
        return 0;
    }

    uint32_t symaddr = (uint32_t)hdr + symtab->sh_offset;
    Elf32_Sym* symbol = &((Elf32_Sym*)symaddr)[i];

    if(symbol->st_shndx == SHN_UNDEF){
        Elf32_Shdr* strtab = elf_section(hdr, symtab->sh_link);
        const char* name = (const char*)hdr + strtab->sh_offset + symbol->st_name;

        void* target = elf_loopup_symbol(name);

        if(target == NULL){
            if(ELF32_ST_BIND(symbol->st_info) & STB_WEAK){
                // Weak symbol initalized as 0
                return 0;
            } else {
                debug_log("[ELF]: Undefined External Symbol: ");
                debug_log(name);
                debug_log("\n");
                return 0;
            }
        } else {
            return (uint32_t)target;
        }

    } else if(symbol->st_shndx == SHN_ABS){
        return symbol->st_value;
    } else {
        Elf32_Shdr* target = elf_section(hdr, symbol->st_shndx);
        return (uint32_t)hdr + symbol->st_value + target->sh_offset;
    }
}*/


/*static uint32_t elf_do_reloc(Elf32_Ehdr* hdr, Elf32_Rel* rel, Elf32_Shdr* reltab){
    Elf32_Shdr* target = elf_section(hdr, reltab->sh_info);

    uint32_t addr = (int)hdr + target->sh_offset;
    uint32_t *ref = (int*)(addr + rel->r_offset);

    uint32_t symval = 0;
    if(ELF32_R_SYM(rel->r_info) != SHN_UNDEF){
        symval = elf_get_symval(hdr, reltab->sh_link, ELF32_R_SYM(regs->r_info));
        if(symval == 0) return 0;
    }

    switch (ELF32_R_TYPE(rel->r_info))
    {
        case ELF_TYPE_NONE:
            break;

        case R_386_32:
            *ref = DO_386_32(symval, *ref);
            break;

        case R_386_PC32:
            *ref = DO_386_PC32(symval, *ref, (int)ref);
            break;
    
        default:
            debug_log("[ELF]: Unsupported relocation type\n");
            break;
    }
    return symval;
}

static bool elf_load_rel_stage1(Elf32_Ehdr* hdr){
    Elf32_Shdr* shdr = elf_sheader(hdr);

    for(uint32_t i = 0; i < hdr->e_shnum; i++){
        Elf32_Shdr* section = &shdr[i];

        if(section->sh_type == SHT_NOBITS){
            if(!section->sh_size) continue;

            if(section->sh_flags & SHF_ALLOC){
                // TODO: Allocate memory
                void* mem = NULL;

                section->sh_offset = (int)mem - (int)hdr;
            }
        }
    }

    return true;
}

static bool elf_load_rel_stage2(Elf32_Ehdr* hdr){
    Elf32_Shdr* shdr = elf_sheader(hdr);

    for(uint32_t i = 0; i < hdr->e_shnum; i++){
        Elf32_Shdr* section = &shdr[i];

        if(section->sh_type == SHT_REL){
            for(uint32_t idx = 0; idx < (section->sh_size / section->sh_entsize); idx++){
                Elf32_Rel* reltab = &((Elf32_Rel*)((int)hdr + section->sh_offset))[idx];

                uint32_t res = elf_do_reloc(hdr, reltab, section);

                if(res == 0){
                    debug_log("[ELF]: Failed failed reloc\n");
                    return false;
                }
            }
        }
    }
    return true;
}

static void* elf_load_rel(Elf32_Ehdr* hdr){
    bool res = elf_load_rel_stage1(hdr);
    if(!res){
        debug_log("[ELF]: Failed to load relocatable executable stage 1\n");
        return NULL;
    }   

    bool res = elf_load_rel_stage2(hdr);
    if(!res){
        debug_log("[ELF]: Failed to load relocatable executable stage 2\n");
        return NULL;
    }   

    // TODO: Load Program

    return (void*)hdr->e_entry;
}*/