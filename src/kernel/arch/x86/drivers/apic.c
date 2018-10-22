#include <rhino/arch/x86/drivers/apic.h>

extern pdirectory* kernel_directory;
pdirectory *subsystemDir, *cr3;
uint64_t* lapic, ioapic;
MADT* madt;

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
    lapic = (uint64_t*)(lo & 0xfffff000);
    pic_disable();
    init_lapic((uint32_t)lapic);

    madt = find_table(MADT_SIGNATURE);

    for(uint32_t ptr = (uint32_t)madt + sizeof(MADT), s = 0; ptr < (uint32_t)madt + mmio_read32(&(madt->h.Length)); ptr += s){
        uint8_t typ = mmio_read8((uint32_t*)ptr);
        if(typ == 0){
            s = 8; // LAPIC
        } else if(typ == 1){
            s = 12;
            ioapic = mmio_read32(((uint32_t*)ptr + 1)); // IOAPIC
            continue;
        } else if(typ == 2) {
            s = 10;
            // interrupt redirect
        } else if(typ == 4) {
            s = 10;
            // IOAPIC NMI
            
        }
    }
    init_ioapic((uint32_t)ioapic);

    for(uint8_t i = 0; i < 16; i++){
        if(i == 2) break; // Skip PIC2 Cascade IRQ, it seems to break things
        ioapic_set_entry(apic_redirect(i), IRQ_BASE + i);
    }
    return true;
}



uint32_t apic_redirect(uint8_t IRQ){
    for(uint32_t ptr = (uint32_t)madt + sizeof(MADT), s = 0; ptr < (uint32_t)madt + mmio_read32(&(madt->h.Length)); ptr += s){
        uint8_t typ = mmio_read8((uint32_t*)ptr);
        if(typ == 0){
            s = 8; // LAPIC
        } else if(typ == 1){
            s = 12; // IOAPIC
        } else if(typ == 2) {
            s = 10;
            if(mmio_read8((uint32_t*)(((uint8_t*)ptr) + 3)) == IRQ) return mmio_read32(((uint32_t*)ptr + 1));
            // interrupt redirect
        } else if(typ == 4) {
            s = 10;
            // IOAPIC NMI
        }
    }
    return IRQ;
}