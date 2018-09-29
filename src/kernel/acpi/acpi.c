#include <rhino/acpi/acpi.h>

RSDP* rsdp;
RSDT* rsdt;
XSDP* xsdp;
XSDT* xsdt;
FADT* fadt;
uint32_t cr3;
pdirectory* subsystemDir;
extern pdirectory* kernel_directory;
bool version2;
static inline uint32_t getCR3(){
    int a;
    asm volatile("movl %%cr3, %%eax; movl %%eax, %0;":"=m"(a)::"%eax");
    return a;
}

static inline void setCR3(uint32_t cr3){
    asm volatile("movl %%cr3, %%eax; movl %%eax, %0;":"=m"(cr3)::"%eax");
}

static void acpi_enter_subsystem(){
    cr3 = getCR3();
    setCR3((uint32_t)subsystemDir);
}

static void acpi_leave_subsystem(){
    setCR3(cr3);
}

/*static bool detectACPI(){
    uint32_t eax, edx;
    cpuid(1, &eax, &edx);
    return BIT_IS_SET(edx, 22);
}
*/


static bool doChecksum(SDTHeader *tableHeader)
{
    unsigned char sum = 0;
    for (uint32_t i = 0; i < tableHeader->Length; i++)
    {
        sum += ((char *) tableHeader)[i];
    }
    return sum == 0;
}

RSDP* find_rsdp(){
    //char *sig = ACPI_RSDT_SIG;
    //RSDP* rsdp;
    for(int i = 0; i < (0x000FFFFF); i += 0x1000) vmm_map_page((void*)i, (void*)i, 0);
    uint8_t* ebda = get_ebda_base();

    for(uint8_t* i = (uint8_t*)(ebda); (uint32_t)i < (uint32_t)(ebda + 1024); i++){
        /*if(memcmp(sig, i, 8) == 0){
            kprint("AAAA");
            rsdp = (RSDP*)i;
        }*/
        if(*i == 'R' && *(i + 1) == 'S' && *(i + 2) == 'D' && *(i + 3) == ' ' && *(i + 4) == 'P' && *(i + 5) == 'T' && *(i + 6) == 'R' && *(i + 7) == ' '){
            rsdp = (RSDP*)i;
        }
    }
    
    for(uint8_t* i = (uint8_t*)(0x000E0000); (uint32_t)i < (uint32_t)(0x00100000); i++){
        /*if(memcmp(sig, i, 8) == 0){
            kprint("AAAAB");
            rsdp = (RSDP*)i;
        }*/
        /*char buf[2] = {*i, '\0'};
        kprint(buf);*/
        if(*i == 'R' && *(i + 1) == 'S' && *(i + 2) == 'D' && *(i + 3) == ' ' && *(i + 4) == 'P' && *(i + 5) == 'T' && *(i + 6) == 'R' && *(i + 7) == ' '){
            rsdp = (RSDP*)i;
        }
        
    }

    if(rsdp != 0){
        uint8_t *bptr = (uint8_t*)rsdp;

        int8_t check = 0;
        for(uint32_t i = 0; i < sizeof(RSDP); i++){
            check += *bptr;
            bptr++;
        }

        if(check == 0){
            return rsdp;
        } else {
            kprint("[ACPI]: Found RSDP but checksum is not valid\n");
            return 0;
        }
    }
    return 0;
}

void init_acpi(){

    subsystemDir = vmm_clone_dir(kernel_directory);
    acpi_enter_subsystem();

    rsdp = find_rsdp();
    if(rsdp == 0){
        kprint_err("[ACPI] Couldn't find RSDP\n");
        return;
    }


    if(rsdp->Revision > 0){
        kprint("[ACPI]: Detected ACPI ver2\n");
        version2 = true;
        uint8_t *bptr = (uint8_t*)rsdp;

        int8_t check = 0;
        for(uint32_t i = 0; i < sizeof(XSDP) - 3; i++){
            check += *bptr;
            bptr++;
        }
        if(check == 0){
            xsdp = (XSDP*)rsdp;
            xsdt = (XSDT*)(uint32_t)xsdp->XsdtAddress;
            vmm_map_page((void*)((uint32_t)xsdt), (void*)((uint32_t)xsdt), 1);
            if(!doChecksum(&xsdt->h)){
                kprint_err("[ACPI]: XSDT Checksum Failed\n");
                return;
            }
        } else {
            kprint_err("[ACPI]: Detected ACPI ver2 but XSDP checksum is not valid, returning");
            return;
        }
    } else {
        rsdt = (RSDT*)rsdp->RsdtAddress;
        vmm_map_page((void*)((uint32_t)rsdp->RsdtAddress), (void*)((uint32_t)rsdp->RsdtAddress), 1);
        if(!doChecksum((SDTHeader*)rsdt)){
            kprint_err("[ACPI]: RSDT Checksum Failed\n");
            return;
        }
    }

    if(!version2){

        
        int entries = (rsdt->h.Length - sizeof(rsdt->h)) / 4;
 
        for (int i = 0; i < entries; i++)
        {
            vmm_map_page((void*)rsdt->PointerToOtherSDT[i], (void*)rsdt->PointerToOtherSDT[i], 0);
            SDTHeader *h = (SDTHeader *) rsdt->PointerToOtherSDT[i];
            for(uint8_t j = 0; j < 4; j++){
                char buf[2] = {h->Signature[j], '\0'};
                kprint(buf);
            }
            kprint("\n");
        }
 

        /*for(uint8_t i = 0; i < 5; i++){
            vmm_map_page((void*)rsdt->PointerToOtherSDT[i], (void*)rsdt->PointerToOtherSDT[i], 0);
            for(uint8_t j = 0; j < 4; j++){
                SDTHeader* h = (SDTHeader*)rsdt->PointerToOtherSDT[i];
                char buf[2] = {h->Signature[j], '\0'};
                kprint(buf);
            }
            kprint("\n");
        }*/
    }

    acpi_leave_subsystem();
    return;
}