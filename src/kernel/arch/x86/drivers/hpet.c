#include <rhino/arch/x86/drivers/hpet.h>
extern pdirectory* kernel_directory;
pdirectory *subsystemDir, *cr3;
HPET* hpet;

uint32_t hpetFrequency;
uint16_t hpetMinimumTick;

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

bool init_hpet(uint64_t frq){
    void* tab = find_table(HPET_ACPI_SIGNATURE);

    subsystemDir = vmm_clone_dir(kernel_directory);
    hpet_enter_subsystem();
    vmm_map_page(tab, tab, 0);
    hpet = (HPET*)tab;

    vmm_map_page((void*)(uint32_t)hpet->address.Address, (void*)(uint32_t)hpet->address.Address, 0);
    uint64_t res;

    res = hpet_read(HPET_GENERAL_CAPABILITIES_REG);
    if(!BIT_IS_SET(res, 15)){ // Check if supports legacy routing
        return false;
    }

    res = hpet_read(HPET_GENERAL_CONFIGURATION_REG);
    BIT_SET(res, 1); // Set Legacy Replacement routing
    BIT_CLEAR(res, 0); // Disable Counter And interrupts;
    hpet_write(HPET_GENERAL_CONFIGURATION_REG, res);


    hpetFrequency = (hpet_read(HPET_GENERAL_CAPABILITIES_REG) >> 32) & 0xFFFFFFFF;
    hpetMinimumTick = hpet->minimum_tick;

    // Set main counter to 0
    //hpet_write(HPET_MAIN_COUNTER_VAL, 0);

    // Initialize Comparators / Timers
    res = hpet_read(HPET_TIMER_CONFIGURATION_AND_CAPABILITY_REG(0));
    BIT_SET(res, 2);
    BIT_SET(res, 3);
    BIT_SET(res, 6);
    hpet_write(HPET_TIMER_CONFIGURATION_AND_CAPABILITY_REG(0), res);

    hpet_write(HPET_TIMER_COMPARATOR_VALUE_REG(0), hpet_read(HPET_MAIN_COUNTER_VAL) + frq);
    hpet_write(HPET_TIMER_COMPARATOR_VALUE_REG(0), frq);


    res = hpet_read(HPET_GENERAL_CONFIGURATION_REG);
    BIT_SET(res, 0);
    hpet_write(HPET_GENERAL_CONFIGURATION_REG, res);

    hpet_leave_subsystem();
    return true;
}