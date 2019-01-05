#include <rhino/acpi/acpi.h>

RSDP* rsdp;
RSDT* rsdt;
XSDP* xsdp;
XSDT* xsdt;
FADT* fadt;

uint32_t slp_typa;
uint32_t slp_typb;

pdirectory* cr3;
pdirectory* subsystemDir;
extern pdirectory* kernel_directory;
bool version2;

static void acpi_enter_subsystem(){
    cr3 = vmm_get_directory();
    vmm_switch_pdirectory(subsystemDir);
    debug_log("[ACPI]: Entered ACPI Subsystem\n");
}

static void acpi_leave_subsystem(){
    vmm_switch_pdirectory(cr3);
    debug_log("[ACPI]: Left ACPI Subsystem\n");
}

bool detectACPI(){
    uint32_t eax, ecx, edx;
    cpuid(1, &eax, &ecx, &edx);
    if(!BIT_IS_SET(edx, 22)){
        if(BIT_IS_SET(ecx, 31)){ // 31 is officialy reserved as 0 but QEMU sets it
            return true;
        } else {
            return false;
        }
    } else {
        return true;
    }
}



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
    RSDP* rsdpval;
    for(int i = 0; i < (0x000FFFFF); i += 0x1000) vmm_map_page((void*)i, (void*)i, 0);
    uint8_t* ebda = get_ebda_base();

    for(uint8_t* i = (uint8_t*)(ebda); (uint32_t)i < (uint32_t)(ebda + 1024); i++){
        if(*i == 'R' && *(i + 1) == 'S' && *(i + 2) == 'D' && *(i + 3) == ' ' && *(i + 4) == 'P' && *(i + 5) == 'T' && *(i + 6) == 'R' && *(i + 7) == ' '){
            rsdpval = (RSDP*)i;
        }
    }
    
    for(uint8_t* i = (uint8_t*)(0x000E0000); (uint32_t)i < (uint32_t)(0x00100000); i++){
        if(*i == 'R' && *(i + 1) == 'S' && *(i + 2) == 'D' && *(i + 3) == ' ' && *(i + 4) == 'P' && *(i + 5) == 'T' && *(i + 6) == 'R' && *(i + 7) == ' '){
            rsdpval = (RSDP*)i;
        }
        
    }

    if(rsdpval != 0){
        uint8_t *bptr = (uint8_t*)rsdpval;

        int8_t check = 0;
        for(uint32_t i = 0; i < sizeof(RSDP); i++){
            check += *bptr;
            bptr++;
        }

        if(check == 0){
            return rsdpval;
        } else {
            kprint("[ACPI]: Found RSDP but checksum is not valid\n");
            return 0;
        }
    }
    return 0;
}

void* find_table_int(char* signature){
    if(!version2){
        int entries = (rsdt->h.Length - sizeof(rsdt->h)) / 4;
        for (int i = 0; i < entries; i++)
        {
            vmm_map_page((void*)rsdt->PointerToOtherSDT[i], (void*)rsdt->PointerToOtherSDT[i], 0);
            SDTHeader *h = (SDTHeader *) rsdt->PointerToOtherSDT[i];
            if(h->Signature[0] == signature[0] && h->Signature[1] == signature[1] && h->Signature[2] == signature[2] && h->Signature[3] == signature[3]){
                if(doChecksum(h)){
                    return (void*)h;
                } else {
                    kprint_err("[ACPI] Found");
                    kprint_err(signature);
                    kprint_err(" but checksum is invalid\n");
                    return 0;
                }
            }
        }
    } else {
        int entries = (xsdt->h.Length - sizeof(xsdt->h)) / 8;
        for (int i = 0; i < entries; i++)
        {
            
            vmm_map_page((void*)(uint32_t)xsdt->PointerToOtherSDT[i], (void*)(uint32_t)xsdt->PointerToOtherSDT[i], 0);
            SDTHeader *h = (SDTHeader *) (uint32_t)xsdt->PointerToOtherSDT[i];
            if(h->Signature[0] == signature[0] && h->Signature[1] == signature[1] && h->Signature[2] == signature[2] && h->Signature[3] == signature[3]){
                if(doChecksum(h)){
                    return (void*)h;
                } else {
                    debug_log("b");
                    kprint_err("[ACPI] Found");
                    kprint_err(signature);
                    kprint_err(" but checksum is invalid\n");
                    return 0;
                }
            }
        }
    }
    debug_log("[ACPI] Could not find ");
    debug_log(signature);
    debug_log("\n");
    return 0;
}


void init_acpi(){
    debug_log("[ACPI]: Starting ACPI\n");
    if(!detectACPI()){
        kprint_warn("[ACPI] No ACPI detected\n");
        debug_log("[ACPI]: No ACPI Detected\n");
        return;
    }

    subsystemDir = vmm_clone_dir(kernel_directory);
    acpi_enter_subsystem();
    
    rsdp = find_rsdp();
    if(rsdp == 0){
        kprint_err("[ACPI] Couldn't find RSDP\n");
        debug_log("[ACPI]: Couldn't find RSDP\n");
        acpi_leave_subsystem();
        return;
    }


    if(rsdp->Revision > 0){
        debug_log("[ACPI]: Detected ACPI Version 2\n");
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
            if(!doChecksum((SDTHeader*)xsdt)){
                kprint_err("[ACPI]: XSDT Checksum Failed\n");
                debug_log("[ACPI]: XSDT Checksum Failed\n");
                acpi_leave_subsystem();
                return;
            }
        } else {
            kprint_err("[ACPI]: XSDP Checksum failed\n");
            debug_log("[ACPI]: XSDP Checksum failed\n");
            acpi_leave_subsystem();
            return;
        }
    } else {
        rsdt = (RSDT*)rsdp->RsdtAddress;
        vmm_map_page((void*)((uint32_t)rsdp->RsdtAddress), (void*)((uint32_t)rsdp->RsdtAddress), 1);
        if(!doChecksum((SDTHeader*)rsdt)){
            kprint_err("[ACPI]: RSDT Checksum Failed\n");
            debug_log("[ACPI]: RSDT Checksum Failed\n");
            acpi_leave_subsystem();
            return;
        }
    }

    fadt = find_table_int("FACP");
    if(!doChecksum((SDTHeader*)fadt)){
        kprint_err("[ACPI]: FADT Checksum Failed\n");
        debug_log("[ACPI]: FADT Checksum Failed\n");
        acpi_leave_subsystem();
        return;
    }
    void* dsdt = (version2) ? ((void*)((uint32_t)fadt->X_Dsdt)) : ((void*)fadt->Dsdt);
    vmm_map_page(dsdt, dsdt, 0);
    for(uint32_t i = 1; i < ((((SDTHeader*)dsdt)->Length / 0x1000) + 1); i++) vmm_map_page((void*)(dsdt + (0x1000 * i)), (void*)(dsdt + (0x1000 * i)), 0);
    SDTHeader* dsdt_header = (SDTHeader*)dsdt;
    if(!doChecksum(dsdt_header)){
        kprint_err("[ACPI]: DSDT Checksum Failed\n");
        debug_log("[ACPI]: DSDT Checksum Failed\n");
        acpi_leave_subsystem();
        return;
    }

    uint8_t *dsdt_ptr = 0;
    uint32_t dsdt_len = 0;
    dsdt_ptr = (uint8_t*)((uint32_t)dsdt_header + sizeof(SDTHeader));
    dsdt_len = dsdt_header->Length - sizeof(SDTHeader);

    while(0 < dsdt_len--){
        if ( memcmp(dsdt_ptr, "_S5_", 4) == 0)
            break;
        dsdt_ptr++;
    }

    if(dsdt_len > 0){
        if ( ( *(dsdt_ptr-1) == 0x08 || ( *(dsdt_ptr-2) == 0x08 && *(dsdt_ptr-1) == '\\') ) && *(dsdt_ptr+4) == 0x12 )
        {
            dsdt_ptr += 5;
            dsdt_ptr += ((*dsdt_ptr &0xC0)>>6) +2;   // calculate PkgLength size

            if (*dsdt_ptr == 0x0A) dsdt_ptr++;   // skip byteprefix
            slp_typa = *(dsdt_ptr)<<10;
            dsdt_ptr++;

            if (*dsdt_ptr == 0x0A) dsdt_ptr++;   // skip byteprefix
            slp_typb = *(dsdt_ptr)<<10;
        } else {
            kprint_err("[ACPI]: Error Parsing \\_S5 Object\n");
            debug_log("[ACPI]: Error Parsing \\_S5 Object\n");
            acpi_leave_subsystem();
            return;
        }
        
    } else {
        kprint_err("[ACPI]: Could not find \\_S5 Object\n");
        debug_log("[ACPI]: Could not find \\_S5 Object\n");
        acpi_leave_subsystem();
        return;
    }



    acpi_leave_subsystem();
    debug_log("[ACPI]: Initialized ACPI\n");
    return;
}

uint32_t acpi_get_fadt_boot_arch_flags(){
    acpi_enter_subsystem();
    uint32_t ret = fadt->BootArchitectureFlags;
    acpi_leave_subsystem();
    return ret;
}

uint32_t acpi_get_fadt_version(){
    acpi_enter_subsystem();
    uint32_t ret = fadt->h.Revision;
    acpi_leave_subsystem();
    return ret;
}

bool acpi_table_exists(char* signature){
    acpi_enter_subsystem();
    void* tab = (void*)find_table_int(signature);
    acpi_leave_subsystem();
    if(tab) return true;
    return false;
}

void* find_table(char* signature){
    acpi_enter_subsystem();
    void* tab = find_table_int(signature);
    acpi_leave_subsystem();
    return tab;
}

bool acpi_reboot(){
    acpi_enter_subsystem();
    debug_log("[ACPI]: Doing ACPI Reset\n");
    if(!BIT_IS_SET(fadt->Flags, 10)){
        acpi_leave_subsystem();
        return false;
    }
    GenericAddressStructure resetReg = fadt->ResetReg;
    if(resetReg.AddressSpace == RHINO_ACPI_ADDRESS_SPACE_SYSTEM_IO){
        outb(resetReg.Address, fadt->ResetValue);
    } else if(resetReg.AddressSpace == RHINO_ACPI_ADDRESS_SPACE_SYSTEM_MEM){
        vmm_map_page((void*)(uint32_t)resetReg.Address, (void*)(uint32_t)resetReg.Address, 0);
        uint32_t f = (uint32_t)resetReg.Address;
        volatile uint8_t* p = (uint8_t*)((uint32_t*)f);
        *p = fadt->ResetValue;
    }
    acpi_leave_subsystem();
    PANIC_M("[ACPI]: REBOOT FAILED\n");
}

void acpi_shutdown(){
    acpi_enter_subsystem();
    uint32_t pm1a_cnt = fadt->PM1aControlBlock;
    uint32_t pm1b_cnt = fadt->PM1bControlBlock;

    if((inw(pm1a_cnt) & 1) == 0){
        if(fadt->SMI_CommandPort != 0 || fadt->AcpiEnable != 0){
            outb(fadt->SMI_CommandPort, fadt->AcpiEnable);
            uint32_t i;
            for(i = 0; i < 300; i++){
                if ( (inw(pm1a_cnt) &1) == 1 )
                  break;
               for(volatile int j = 0; j < 1000; j++);
            }
            if (pm1b_cnt != 0){
                for (; i<300; i++ )
                {
                    if ( (inw((unsigned int) pm1b_cnt) &1) == 1 )
                        break;
                    for(volatile int j = 0; j < 1000; j++);
                }
            }
            if(!(i < 300)){
                kprint_err("[ACPI]: Shutdown Failed\n");
                debug_log("[ACPI]: Shutdown Failed\n");
                acpi_leave_subsystem();

            }
        }
    }

    outw(pm1a_cnt, slp_typa | (1 << 13));
    if(pm1b_cnt != 0) outw(pm1b_cnt, slp_typb | (1 << 13));
    acpi_leave_subsystem();
    PANIC_M("[ACPI]: Shutdown Failed\n");
    return;
}