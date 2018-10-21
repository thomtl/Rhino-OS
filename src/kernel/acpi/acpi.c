#include <rhino/acpi/acpi.h>

RSDP* rsdp;
RSDT* rsdt;
XSDP* xsdp;
XSDT* xsdt;
FADT* fadt;
MADT* madt;
pdirectory* cr3;
pdirectory* subsystemDir;
extern pdirectory* kernel_directory;
bool version2;

static void acpi_enter_subsystem(){
    cr3 = vmm_get_directory();
    vmm_switch_pdirectory(subsystemDir);
}

static void acpi_leave_subsystem(){
    vmm_switch_pdirectory(cr3);
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
    } else if(version2){
        int entries = (xsdt->h.Length - sizeof(xsdt->h)) / 8;
 
        for (int i = 0; i < entries; i++)
        {
            vmm_map_page((void*)(uint32_t)xsdt->PointerToOtherSDT[i], (void*)(uint32_t)xsdt->PointerToOtherSDT[i], 0);
            SDTHeader *h = (SDTHeader *) (uint32_t)xsdt->PointerToOtherSDT[i];
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
    }
    #ifdef DEBUG
    kprint_err("[ACPI] Could not find ");
    kprint_err(signature);
    #endif
    return 0;
}


void init_acpi(){
    if(!detectACPI()){
        kprint_warn("[ACPI] No ACPI detected\n");
        return;
    }

    subsystemDir = vmm_clone_dir(kernel_directory);
    acpi_enter_subsystem();

    rsdp = find_rsdp();
    if(rsdp == 0){
        kprint_err("[ACPI] Couldn't find RSDP\n");
        acpi_leave_subsystem();
        return;
    }


    if(rsdp->Revision > 0){
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
                acpi_leave_subsystem();
                return;
            }
        } else {
            kprint_err("[ACPI]: XSDP Checksum failed");
            acpi_leave_subsystem();
            return;
        }
    } else {
        rsdt = (RSDT*)rsdp->RsdtAddress;
        vmm_map_page((void*)((uint32_t)rsdp->RsdtAddress), (void*)((uint32_t)rsdp->RsdtAddress), 1);
        if(!doChecksum((SDTHeader*)rsdt)){
            kprint_err("[ACPI]: RSDT Checksum Failed\n");
            acpi_leave_subsystem();
            return;
        }
    }


    fadt = find_table_int("FACP");
    if(!doChecksum((SDTHeader*)fadt)){
        kprint_err("[ACPI]: FADT Checksum Failed\n");
        acpi_leave_subsystem();
        return;
    }

    /*madt = find_table_int("APIC");
    if(madt != 0){
        uint32_t s = 0;
        for(uint32_t ptr = (uint32_t)madt + sizeof(MADT); ptr < (uint32_t)madt + madt->h.Length; ptr += s){
            uint8_t* typ = (uint8_t*)ptr;
            if(*typ == 0){
                s = 8;
                kprint("LAPIC: ");
                uint8_t apic_id = *(typ + 3);
                uint8_t cpu_id = *(typ + 2);
                char buf[5] ="";
                int_to_ascii(cpu_id, buf);
                kprint("cpuID=");
                kprint(buf);
                kprint(", ");
                int_to_ascii(apic_id, buf);
                kprint("lapicID=");
                kprint(buf);
                
                if(BIT_IS_SET(*(typ + 4), 0)){
                    kprint(", ");
                    kprint("Enabled");
                }
                kprint("\n");
            } else if(*typ == 1){
                s = 12;
                kprint("I/OAPIC");
                uint8_t apic_id = *(typ + 2);
                char buf[10] ="";
                int_to_ascii(apic_id, buf);
                kprint("ioapicID=");
                kprint(buf);
                kprint(", ");
                uint32_t* t = (uint32_t*)typ;
                uint32_t addr = *(t + 1);
                for(int i = 0; i < 10; i++) buf[i] = '\0';
                hex_to_ascii(addr, buf);
                kprint("addr=");
                kprint(buf);
                kprint(", ");

                addr = *(t + 2);
                int_to_ascii(addr, buf);
                kprint("GSIbase=");
                kprint(buf);
                kprint(", ");
                kprint("\n");
            } else if(*typ == 2) {
                s = 10;
                kprint("Interruptible Source Override");
                uint8_t source_id = *(typ + 3);
                char buf[10] ="";
                int_to_ascii(source_id, buf);
                kprint(" PICIRQ=");
                kprint(buf);
                kprint(", ");
                uint32_t* t = (uint32_t*)typ;
                uint32_t gsi = *(t + 1);
                int_to_ascii(gsi, buf);
                kprint(" GSI=");
                kprint(buf);
                kprint("\n");
            } else if(*typ == 4) {
                s = 10;
                kprint("LAPIC NMI");
                uint8_t lintn = *(typ + 2);
                uint8_t cpu_id = *(typ + 5);
                char buf[5] ="";
                int_to_ascii(cpu_id, buf);
                kprint("cpuID=");
                kprint(buf);
                kprint(", ");
                int_to_ascii(lintn, buf);
                kprint("lintnid=");
                kprint(buf);
                kprint("\n");
            } else {
                kprint("Unkown MADT id");
                char buf[25] = "";
                hex_to_ascii((uint32_t)*typ, buf);
                kprint(buf);

                acpi_leave_subsystem();
                return;
            }
        }
    }*/

    acpi_leave_subsystem();
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
    if(!BIT_IS_SET(fadt->Flags, 10)){
        acpi_leave_subsystem();
        return false;
    }
    GenericAddressStructure resetReg = fadt->ResetReg;
    if(resetReg.AddressSpace == PHI_ACPI_ADDRESS_SPACE_SYSTEM_IO){
        outb(resetReg.Address, fadt->ResetValue);
    } else if(resetReg.AddressSpace == PHI_ACPI_ADDRESS_SPACE_SYSTEM_MEM){
        vmm_map_page((void*)(uint32_t)resetReg.Address, (void*)(uint32_t)resetReg.Address, 0);
        uint32_t f = (uint32_t)resetReg.Address;
        volatile uint8_t* p = (uint8_t*)((uint32_t*)f);
        *p = fadt->ResetValue;
    }
    acpi_leave_subsystem();
    PANIC_M("[ACPI]: REBOOT FAILED\n");
}