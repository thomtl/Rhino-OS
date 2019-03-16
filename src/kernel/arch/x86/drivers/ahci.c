#include <rhino/arch/x86/drivers/ahci.h>

ahci_device_t ahci_devices[32];

spinlock_mutex_t ahci_mutex;

uint32_t ahci_control_sets_n = 0;

uint32_t ahci_ports_implemented = 0; // bitmap

static inline uint32_t ahci_read(uint32_t reg){
    if(reg > AHCI_HBA_MAX_SIZE){
        debug_log("[AHCI]: Trying to read register from outside the HBA\n");
        return 0;
    }
    return mmio_read32((uint32_t*)(ahci_devices[0].base + reg));
}

static inline void ahci_write(uint32_t reg, uint32_t val){
    if(reg > AHCI_HBA_MAX_SIZE){
        debug_log("[AHCI]: Trying to read register from outside the HBA\n");
        return;
    }
    mmio_write32((uint32_t*)(ahci_devices[0].base + reg), val);
}

static bool ahci_bios_os_handoff(){
    uint32_t version = ahci_read(AHCI_VS);
    if(version >= 0x00010200){ // if later than 1.2
        uint32_t cap2 = ahci_read(AHCI_CAP2);
        if(BIT_IS_SET(cap2, AHCI_CAP2_BOH)){
            uint32_t bohc = ahci_read(AHCI_BOHC);
            ahci_write(AHCI_BOHC, bohc | (1 << AHCI_BOHC_OOS));

            msleep(25);

            bohc = ahci_read(AHCI_BOHC);
            if(BIT_IS_SET(bohc, AHCI_BOHC_BB)) msleep(2000);

            bohc = ahci_read(AHCI_BOHC);
            if((bohc & ((1 << AHCI_BOHC_BB) | (1 << AHCI_BOHC_OOS) | (1 << AHCI_BOHC_BOS))) != (1 << 1)) return false;

            ahci_write(AHCI_BOHC, bohc);
        }
    }
    return true;
}

static bool ahci_comreset(ahci_device_t dev){
    if(!dev.exists){
        debug_log("[AHCI]: Device does not exist and is no candidate for a COMRESET\n");
        return false;
    }

    ahci_write(AHCI_PxCMD(dev.drive_num), ((ahci_read(AHCI_PxCMD(dev.drive_num)) & 0xFFFFFFFE) | 0)); // Stop Engine
    msleep(100);

    ahci_write(AHCI_PxSCTL(dev.drive_num), 0x301);//((ahci_read(AHCI_PxSCTL(dev.drive_num)) & 0xFFFFFFF0) | 1));
    msleep(1);
    ahci_write(AHCI_PxSCTL(dev.drive_num), 0x300);//((ahci_read(AHCI_PxSCTL(dev.drive_num)) & 0xFFFFFFF0) | 0));
    uint32_t timeout = 1000;
    while(timeout > 0){
        uint32_t ssts = ahci_read(AHCI_PxSSTS(dev.drive_num));
        if((ssts & 0xF) == 0b11){
            ahci_write(AHCI_PxSERR(dev.drive_num), 0xFFFFFFFF);
            return true;
        } 
        msleep(1);
        timeout--;
    }

    //debug_log("[AHCI]: COMRESET timed out\n");
    return false;
}


// Implement Staggerd spinup before using
/*static bool ahci_reset(){
    ahci_write(AHCI_GHC, (1 << AHCI_GHC_HR));
    msleep(2);
    uint32_t timeout = 1000;
    while(timeout > 0){
        
        if(BIT_IS_CLEAR(ahci_read(AHCI_GHC), AHCI_GHC_HR)){
            return true;
        }
        msleep(1);
        timeout--;
    }
    debug_log("[AHCI]: reset timed out\n");
    return false;
}*/

void ahci_init(uint16_t bus, uint16_t device, uint8_t function){
    debug_log("[AHCI]: Initializing Driver\n");
    if(bus > PCI_BUS_N || device > PCI_DEVICE_N || function > PCI_FUNCTION_N){
        debug_log("[AHCI]: PCI Parameters are outside of legal ranges\n");
        return;
    }
    acquire_spinlock(&ahci_mutex);

    uint32_t ahci_base = pci_read_bar(bus, device, function, PCI_BAR_5) & 0xFFFFFFF0;

    for(uint8_t i = 0; i < 32; i++){
        ahci_devices[i].base = ahci_base;
        ahci_devices[i].drive_num = i;
    }

    if(!ahci_bios_os_handoff()){
        debug_log("[AHCI]: BIOS/OS Handoff failed\n");
        release_spinlock(&ahci_mutex);
        return;
    }

    if(BIT_IS_CLEAR(ahci_read(AHCI_CAP), AHCI_CAP_SAM)) ahci_write(AHCI_GHC, (1 << AHCI_GHC_EA));

    /*if(!ahci_reset()){
        debug_log("[AHCI]: Failed reset\n");
        release_spinlock(&ahci_mutex);
        return;
    }*/

    ahci_control_sets_n = (ahci_read(AHCI_CAP) & 0x1F) + 1;
    ahci_ports_implemented = ahci_read(AHCI_PI);

    for(uint8_t i = 0; i < 32; i++){
        uint32_t version = ahci_read(AHCI_VS);
        ahci_devices[i].major_version = version & 0xFFFF;
        ahci_devices[i].minor_version = (version >> AHCI_VS_MINOR) & 0xFFFF;
        uint32_t capibilities = ahci_read(AHCI_CAP);
        ahci_devices[i].command_list_items = ((capibilities >> AHCI_CAP_NCS) & 0x1F) + 1; // Zero based number
    }

    for(uint8_t i = 0; i < 32; i++){
        if(BIT_IS_SET(ahci_ports_implemented, i)){
            ahci_devices[i].exists = true;

            if((ahci_read(AHCI_PxSSTS(i)) & 0xF) == 0){
                ahci_devices[i].exists = false;
                continue;
            }

            if(!ahci_comreset(ahci_devices[i])){
                //debug_log("[AHCI]: Device failed COMRESET\n");
                ahci_devices[i].exists = false;
                continue;
            }

            uint32_t signature = ahci_read(AHCI_PxSIG(i));
            if(signature == AHCI_SIGNATURE_SATAPI) ahci_devices[i].atapi = true;
            else if(signature == AHCI_SIGNATURE_SATA) ahci_devices[i].atapi = false;
            else {
                debug_log("[AHCI]: Device with unkown signature, SIG: ");
                debug_log_number_hex(signature);
                debug_log("\n");
                ahci_devices[i].exists = false;
                continue;
            }

            ahci_devices[i].exists = true;
        } else {
            ahci_devices[i].exists = false;
        }
    }

    

    debug_log("[AHCI]: Driver Initialized\n");
    release_spinlock(&ahci_mutex);
    return;
}