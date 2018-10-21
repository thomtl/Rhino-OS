#include <rhino/arch/x86/drivers/apic.h>

extern pdirectory* kernel_directory;
pdirectory *subsystemDir, *cr3;
uint64_t* lapic, ioapic;
MADT* madt;

static void apic_enter_subsystem(){
    cr3 = vmm_get_directory();
    vmm_switch_pdirectory(subsystemDir);
}

static void apic_leave_subsystem(){
    vmm_switch_pdirectory(cr3);
}


static void apic_write(uint64_t reg, uint64_t val){
    volatile uint64_t* ptr = (uint64_t*)(uint32_t)(lapic + reg);
    *ptr = val;
}

static uint64_t apic_read(uint64_t reg){
    volatile uint64_t* ptr = (uint64_t*)(uint32_t)(lapic + reg);
    return *ptr;
}

bool detect_apic(){
    uint32_t a, c, d;
    cpuid(CPUID_GET_FEATURES, &a, &c, &d);
    if(BIT_IS_SET(d, CPUID_FEAT_EDX_APIC)){
        return true;
    }
    return false;
}

bool init_apic(){
    if(!detect_apic()) return false;
    
    uint32_t hi, lo;
    msr_read(APIC_BASE, &lo, &hi);
    madt = find_table(MADT_SIGNATURE);
    if(madt == 0) return false; // There is no MADT(APIC) table, Abort
    subsystemDir = vmm_clone_dir(kernel_directory);
    apic_enter_subsystem();

    vmm_map_page(madt, madt, 0);
    lapic = (uint64_t*)(lo & 0xFFFFF000);//(msr >> 12) & 0xFFFFFFFFFF;
    vmm_map_page(lapic, lapic, 0);

    pic_disable();

    apic_write(LAPIC_SPURIOUS_VECTOR_REG, apic_read(LAPIC_SPURIOUS_VECTOR_REG) | 0x100 | 0xFF);
    uint32_t s = 0;
    for(uint32_t ptr = (uint32_t)madt + sizeof(MADT); ptr < (uint32_t)madt + madt->h.Length; ptr += s){
        uint8_t* typ = (uint8_t*)ptr;
        if(*typ == 0){
            s = 8;
            uint8_t apic_id = *(typ + 3);
            uint8_t cpu_id = *(typ + 2);
        } else if(*typ == 1){
            s = 12;
            uint8_t apic_id = *(typ + 2);
            uint32_t addr = *((uint32_t*)typ + 1);
        } else if(*typ == 2) {
            s = 10;
            uint8_t source_id = *(typ + 3);
            uint32_t* t = (uint32_t*)typ;
            uint32_t gsi = *(t + 1);
        } else if(*typ == 4) {
            s = 10;
            uint8_t lintn = *(typ + 2);
            uint8_t cpu_id = *(typ + 5);
        }
    }


    apic_leave_subsystem();
    return true;
}

void apic_send_eoi(){
    apic_write(LAPIC_EOI_REG, 0);
}