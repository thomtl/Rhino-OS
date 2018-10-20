#include <rhino/arch/x86/drivers/hpet.h>

extern pdirectory* kernel_directory;
pdirectory *subsystemDir, *cr3;
HPET* hpet;

uint32_t hpetFrequency;
uint16_t hpetMinimumTick;
uint8_t hpetNCompartators;

static void hpet_enter_subsystem(){
    cr3 = vmm_get_directory();
    vmm_switch_pdirectory(subsystemDir);
}

static void hpet_leave_subsystem(){
    vmm_switch_pdirectory(cr3);
}

static void hpet_write(uint64_t reg, uint64_t val){
    volatile uint64_t* ptr = (uint64_t*)(uint32_t)(hpet->address.Address + reg);
    *ptr = val;
}

static uint64_t hpet_read(uint64_t reg){
    volatile uint64_t* ptr = (uint64_t*)(uint32_t)(hpet->address.Address + reg);
    return *ptr;
}

bool detect_hpet(){
    return acpi_table_exists(HPET_ACPI_SIGNATURE);
}

void hpet_init_timer(uint32_t n, uint64_t frq, bool periodicTimer){
    if(n > hpetNCompartators){
        kprint_err("[HPET]: Trying to access non existant Comparator\n");
    }
    uint64_t res = hpet_read(HPET_TIMER_CONFIGURATION_AND_CAPABILITY_REG(n));
    BIT_SET(res, 2);
    if(!BIT_IS_SET(res, 4) && periodicTimer){
        kprint_err("[HPET]: Selected timer does not support periodic mode\n");
        return;
    }
    if(periodicTimer){
        BIT_SET(res, 3);
        BIT_SET(res, 6);
    }

    hpet_write(HPET_TIMER_CONFIGURATION_AND_CAPABILITY_REG(n), res);

    hpet_write(HPET_TIMER_COMPARATOR_VALUE_REG(n), hpet_read(HPET_MAIN_COUNTER_VAL) + frq);
    if(periodicTimer) hpet_write(HPET_TIMER_COMPARATOR_VALUE_REG(n), frq);
}

bool init_hpet(uint64_t frq){
    void* tab = find_table(HPET_ACPI_SIGNATURE);

    subsystemDir = vmm_clone_dir(kernel_directory);
    hpet_enter_subsystem();
    vmm_map_page(tab, tab, 0);
    hpet = (HPET*)tab;

    vmm_map_page((void*)(uint32_t)hpet->address.Address, (void*)(uint32_t)hpet->address.Address, 0);
    uint64_t res;

    res = hpet_read(HPET_GENERAL_CAPABILITIES_REG);
    if((res & 0xFF) == 0){
        return false; // Non existant HPET revision
    }

    if(!BIT_IS_SET(res, 15)){ // Check if supports legacy routing
        return false;
    }

    hpetNCompartators = ((res >> 8) & 0xF) + 1; // it's one less than it is

    res = hpet_read(HPET_GENERAL_CONFIGURATION_REG);
    BIT_SET(res, 1); // Set Legacy Replacement routing
    BIT_CLEAR(res, 0); // Disable Counter And interrupts;
    hpet_write(HPET_GENERAL_CONFIGURATION_REG, res);


    hpetFrequency = (hpet_read(HPET_GENERAL_CAPABILITIES_REG) >> 32) & 0xFFFFFFFF;
    hpetMinimumTick = hpet->minimum_tick;

    // Set main counter to 0
    hpet_write(HPET_MAIN_COUNTER_VAL, 0);

    // Initialize Comparators / Timers
    hpet_init_timer(0, frq, true);

    res = hpet_read(HPET_GENERAL_CONFIGURATION_REG);
    BIT_SET(res, 0); // Enable Interrupts
    hpet_write(HPET_GENERAL_CONFIGURATION_REG, res);

    hpet_leave_subsystem();
    return true;
}