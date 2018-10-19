#include <rhino/arch/x86/drivers/bga.h>

uint32_t lfb = VBE_DISPI_LFB_PHYSICAL_ADDRESS;
bool bgaActive = false;


static void bga_write(uint16_t index, uint16_t data){
    if(bgaActive){
        outw(VBE_DISPI_IOPORT_INDEX, index);
        outw(VBE_DISPI_IOPORT_DATA, data);
    }
}

static uint16_t bga_read(uint16_t index){
    if(bgaActive){
        outw(VBE_DISPI_IOPORT_INDEX, index);
        return inw(VBE_DISPI_IOPORT_DATA);
    }
    return 0;
}

static inline uint16_t bga_get_version(){
    return bga_read(VBE_DISPI_INDEX_ID);
}

void init_bga(uint8_t bus, uint8_t device, uint8_t function){
    bgaActive = true;
    uint32_t bar0 = pci_read_bar(bus, device, function, PCI_BAR_0);
    if(bar0 == 0){
        kprint_warn("[BGA]: No LFB found in BAR0 falling back to 0xE0000000\n");
    } else {
        lfb = bar0 & 0xFFFFFFF0;
    }
    
    if(lfb == 0xC0000000){
        for (uint32_t i = lfb, j = 0xE0000000; i <= lfb + 0xFF0000; i += 0x1000, j += 0x1000) {
            vmm_map_page((void*)i, (void*)j, 0);
        }
        lfb = 0xE0000000;
    } else {
        for (uint32_t i = lfb; i <= lfb + 0xFF0000; i += 0x1000) {
            vmm_map_page((void*)i, (void*)i, 0);
        }
    }

    

    /*if(bga_get_version() != VBE_DISPI_ID4){
        kprint_err("[BGA]: Didn't detect BGA version 4 aborting\n");
        return;
    }*/
}

uint32_t bga_get_id(){
    uint32_t ret = 0;
    ret = BGA_VENDOR_ID;
    BIT_SET(ret, 16);
    BIT_SET(ret, 17);
    BIT_SET(ret, 18);
    return ret;
}

void bga_set_video_mode(udi_video_mode_set_t* mode){
    if(bgaActive){
        bga_write(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);
        bga_write(VBE_DISPI_INDEX_XRES, mode->width);
        bga_write(VBE_DISPI_INDEX_YRES, mode->height);
        bga_write(VBE_DISPI_INDEX_BPP, mode->bpp);
        bga_write(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_ENABLED | VBE_DISPI_LFB_ENABLED | (mode->clearFramebuffer ? 0 : VBE_DISPI_NOCLEARMEM));
        mode->success = true;
        return;
    }
    mode->success = false;
    return;
}

void bga_switch_banks(uint32_t bank){
    if(bgaActive){
        bga_write(VBE_DISPI_INDEX_BANK, bank);
    }
}

bool bga_is_active(){
    return bgaActive;
}
uint32_t bga_get_lfb(){
    return lfb;
}