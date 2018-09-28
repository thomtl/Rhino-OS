#include <rhino/arch/x86/drivers/bga.h>

uint32_t lfb = VBE_DISPI_LFB_PHYSICAL_ADDRESS;

static void bga_write(uint16_t index, uint16_t data){
    outw(VBE_DISPI_IOPORT_INDEX, index);
    outw(VBE_DISPI_IOPORT_DATA, data);
}

static uint16_t bga_read(uint16_t index){
    outw(VBE_DISPI_IOPORT_INDEX, index);
    return inw(VBE_DISPI_IOPORT_DATA);
}

static inline uint16_t bga_get_version(){
    return bga_read(VBE_DISPI_INDEX_ID);
}

void init_bga(uint8_t bus, uint8_t device, uint8_t function){
    uint32_t bar0 = pci_read_bar(bus, device, function, PCI_BAR_0);
    if(bar0 == 0){
        kprint_warn("[BGA]: No LFB found in BAR0 falling back to 0xE0000000\n");
    } else {
        lfb = bar0;
    }

    /*if(bga_get_version() != VBE_DISPI_ID4){
        kprint_err("[BGA]: Didn't detect BGA version 4 aborting\n");
        return;
    }*/
}

void bga_set_video_mode(uint32_t width, uint32_t height, uint32_t bpp, bool useLFB, bool clearVideoMemory){
    bga_write(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);
    bga_write(VBE_DISPI_INDEX_XRES, width);
    bga_write(VBE_DISPI_INDEX_YRES, height);
    bga_write(VBE_DISPI_INDEX_BPP, bpp);
    bga_write(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_ENABLED | (useLFB ? VBE_DISPI_LFB_ENABLED : 0) | (clearVideoMemory ? 0 : VBE_DISPI_NOCLEARMEM));
}