#include <rhino/arch/x86/drivers/ata.h>

ata_device ata_channels[4];

ata_device* boot_dev;

mutex_t ata_lock;

uint8_t cur_selected = 0xFF;

volatile uint8_t ata_primary_interrupt_fired = 0;
volatile uint8_t ata_secondary_interrupt_fired = 0;

prdt_t ata_primary_prdt __attribute__ ((aligned (4)));
prdt_t ata_secondary_prdt __attribute__ ((aligned (4)));

static void ata_primary_interrupt_handler(registers_t *r){
    ata_primary_interrupt_fired = 1;

    
    inb(ata_channels[0].cmd_addr + ATA_STATUS);
    UNUSED(r);
}

static void ata_secondary_interrupt_handler(registers_t *r){
    ata_secondary_interrupt_fired = 1;
    inb(ata_channels[2].cmd_addr + ATA_STATUS);
    UNUSED(r);
}

static bool ata_wait_for_primary_interrupt(uint32_t timeout){
    
    while(timeout--){
        if(ata_primary_interrupt_fired == 1) return true;
        msleep(1);
    }
    ata_primary_interrupt_fired = 0;
    debug_log("[ATA]: Wait for primary interrupt has timed out\n");
    return false;
}

static bool wait_for_secondary_interrupt(uint32_t timeout){
    
    while(timeout--){
        if(ata_secondary_interrupt_fired == 1) return true;
        msleep(1);
    }
    ata_secondary_interrupt_fired = 0;
    debug_log("[ATA]: Wait for secondary interrupt has timed out\n");
    return false;
}

static bool ata_wait_for_interrupt(ata_device dev, uint32_t timeout){
    if(dev.secondary) return wait_for_secondary_interrupt(timeout);
    else return ata_wait_for_primary_interrupt(timeout);
}

static void ata_clear_interrupt_status(ata_device dev){
    (dev.secondary) ? (ata_secondary_interrupt_fired = 0) : (ata_primary_interrupt_fired = 0);
}

static ata_chs_t lba_to_chs(ata_device dev, uint64_t lba){
    ata_chs_t chs;
    uint32_t hpc, spt;
    hpc = dev.identity[3];
    spt = dev.identity[6];

    chs.cylinder = lba / (hpc * spt);
    chs.head = (lba / spt) % hpc;
    chs.sector = (lba % spt) + 1;
    return chs;
}

void ata_init(uint16_t bus, uint8_t device, uint8_t function){
    debug_log("[ATA]: Initializing ATA Driver\n");

    acquire_spinlock(&ata_lock);

    if(bus > PCI_BUS_N || device > PCI_DEVICE_N || function > PCI_FUNCTION_N){
        kprint("[ATA]: PCI Parameters invalid\n");
        return;
    }

    uint8_t ata_power_managment_list = pci_config_read(bus, device, function, 0x34, PCI_SIZE_BYTE);
    if(ata_power_managment_list != 0x0){
        uint16_t word = pci_config_read(bus, device, function, ata_power_managment_list + 2, PCI_SIZE_WORD);
        BIT_CLEAR(word, 0);
        BIT_CLEAR(word, 1);
        BIT_SET(word, 15);
        pci_config_write(bus, device, function, ata_power_managment_list + 2, PCI_SIZE_WORD, word);
        msleep(10); // Sleep for the 10 miliseconds
        

        word = pci_config_read(bus, device, function, ata_power_managment_list + 2, PCI_SIZE_WORD);
        if(BIT_IS_CLEAR(word, 0) && BIT_IS_CLEAR(word, 0)){
        } else {
            debug_log("[ATA]: Device was not able to be set back into D0 powermode\n");
            kprint("[ATA]: Device was not able to be set back into D0 powermode\n");
            return;
        }
    }

    register_interrupt_handler(IRQ14, ata_primary_interrupt_handler);
    register_interrupt_handler(IRQ15, ata_secondary_interrupt_handler);

    pci_config_write(bus, device, function, 0x3C, PCI_SIZE_BYTE, 14);
    pci_config_write(bus, device, function, 0x04, PCI_SIZE_WORD, 0x0005);
    //deregister_interrupt_handler(IRQ14);
    //deregister_interrupt_handler(IRQ15);

    uint32_t bar0 = pci_read_bar(bus, device, function, PCI_BAR_0) & 0xFFFFFFFC;
    uint32_t bar1 = pci_read_bar(bus, device, function, PCI_BAR_1) & 0xFFFFFFFC;
    uint32_t bar2 = pci_read_bar(bus, device, function, PCI_BAR_2) & 0xFFFFFFFC;
    uint32_t bar3 = pci_read_bar(bus, device, function, PCI_BAR_3) & 0xFFFFFFFC;

    if(BIT_IS_SET(pci_get_programming_interface(bus, device, function), 7)){
        uint32_t bar4 = pci_read_bar(bus, device, function, PCI_BAR_4) & 0xFFFFFFFC;
        ata_channels[0].bus_master_dma = true;
        ata_channels[0].bus_master_addr = bar4;
        ata_channels[1].bus_master_dma = true;
        ata_channels[1].bus_master_addr = bar4;
        ata_channels[2].bus_master_dma = true;
        ata_channels[2].bus_master_addr = bar4;
        ata_channels[3].bus_master_dma = true;
        ata_channels[3].bus_master_addr = bar4;
    } else {
        ata_channels[0].bus_master_dma = false;
        ata_channels[1].bus_master_addr = false;
        ata_channels[2].bus_master_dma = false;
        ata_channels[3].bus_master_addr = false; 
    }

    if(bar0 == 0x0) bar0 = 0x1F0; // If not defined set ISA values
    if(bar1 == 0x0) bar1 = 0x3f4;
    if(bar2 == 0x0) bar2 = 0x170;
    if(bar3 == 0x0) bar3 = 0x374;

    ata_channels[0].cmd_addr = bar0;//0x1f0;
    ata_channels[0].cntrl_addr = bar1;//0x3f4;
    ata_channels[0].slave = false;
    ata_channels[0].secondary = false;

    ata_channels[1].cmd_addr = bar0;
    ata_channels[1].cntrl_addr = bar1;
    ata_channels[1].slave = true;
    ata_channels[1].secondary = false;

    ata_channels[2].cmd_addr = bar2;//0x170;
    ata_channels[2].cntrl_addr = bar3;//0x374;
    ata_channels[2].slave = false;
    ata_channels[2].secondary = true;

    ata_channels[3].cmd_addr = bar2;
    ata_channels[3].cntrl_addr = bar3;
    ata_channels[3].slave = true;
    ata_channels[3].secondary = true;

    ata_init_device(&ata_channels[0]);
    ata_init_device(&ata_channels[1]);
    ata_init_device(&ata_channels[2]);
    ata_init_device(&ata_channels[3]);

    /*multiboot_info_t* mbd = get_mbd();

    char buf[25] = "";
    hex_to_ascii(mbd->boot_device, buf);
    kprint(buf);

    if(BIT_IS_SET(mbd->flags, 1)){
        uint8_t bios_dev = (mbd->boot_device >> 24) & 0xFF;
        if(BIT_IS_SET(bios_dev, 7)){

            BIT_CLEAR(bios_dev, 7);

            char buff[25] = "";
            hex_to_ascii(bios_dev, buff);
            kprint(buff);

            if(bios_dev <= 4){
                boot_dev = &ata_channels[bios_dev];
            } else {
                debug_log("[ATA]: BIOS Boot Device extends beyond standardised ATA range\n");
            }
        } else {
            debug_log("[ATA]: BIOS Boot Device is not an ATA device\n It is probably a floppy disk or something else");
        }
    } else {
        debug_log("[ATA]: BIOS Boot Device is not a BIOS device. Manual scan not yet implemented\n");
    }*/

    release_spinlock(&ata_lock);
    
    debug_log("[ATA]: ATA Driver Initialized\n");
}

bool ata_init_device(ata_device* dev){
    
    if(inb(dev->cmd_addr + ATA_STATUS) == 0xFF){
        debug_log("[ATA] No Device");
        dev->exists = false;
        return false;
    }
    
    outb(dev->cmd_addr + ATA_SECT_CNT, 0x2A);
    outb(dev->cmd_addr + ATA_SECT_NUM, 0x55);
    
    msleep(15);
    
    uint8_t a = inb(dev->cmd_addr + ATA_SECT_CNT);
    uint8_t b = inb(dev->cmd_addr + ATA_SECT_NUM);

    if((a != 0x2A) && (b == 0x55)){
        debug_log("[ATA]: No ATA controller?\n");
        dev->exists = false;
        return false;
    }
    

    outb(dev->cmd_addr + ATA_DRV_HEAD, (dev->slave) ? (1 << 4) : (0));
    ata_io_wait();
    outb(dev->cntrl_addr + ATA_DEV_CNTR, (1<<2));
    msleep(1);
    outb(dev->cntrl_addr + ATA_DEV_CNTR, (1<<1));
    msleep(2);
    if(!ata_wait_busy(*dev, 100)){
        debug_log("[ATA] No Device\n");
        dev->exists = false;
        return false;
    }
    msleep(5);

    inb(dev->cmd_addr + ATA_ERROR);
    /*if(inb(primary_master.cmd_addr + ATA_ERROR) != 0){
        kprint("[ATA]: Error not 0 after reset");
        return;
    }*/
    
    outb(dev->cntrl_addr + ATA_DEV_CNTR, 0); // Enable interrupts

    dev->id_high = inb(dev->cmd_addr + ATA_LBA_HIGH);
    dev->id_mid = inb(dev->cmd_addr + ATA_LBA_MID);
    if(((dev->id_high == 0xFF) && (dev->id_mid == 0xFF))){
        debug_log("[ATA]: No Device\n");
        dev->exists = false;
        return false;
    }
    else if(((dev->id_high == 0x00) && (dev->id_mid == 0x00)) || ((dev->id_high == 0xc3) && (dev->id_mid == 0x3c))) dev->atapi = false;
    else if(((dev->id_high == 0xEB) && (dev->id_mid == 0x14)) || ((dev->id_high == 0x96) && (dev->id_mid == 0x69))) dev->atapi = true;
    else {
        debug_log("[ATA]: Unkown Device, LBA_HIGH: ");
        char buf[25] = "";
        hex_to_ascii(dev->id_high, buf);
        debug_log(buf);
        debug_log(", LBA_MID: ");
        char buff[25] = "";
        hex_to_ascii(dev->id_mid, buff);
        append(buf, '\n');
        debug_log(buff);
        dev->exists = false;
        return false;
    }

    if(!ata_identify_16(*dev, dev->identity, dev->atapi)){
        debug_log("[ATA]: No info block\n");
        dev->exists = false;
        return false;
    }


    if(!ata_check_identity(*dev)){
        kprint("[ATA]: Info block invalid\n");
        dev->exists = false;
        return false;
    }

    uint8_t buf_32[512] = {0};
    if(!ata_identify_32(*dev, (uint16_t*)buf_32, dev->atapi)){
        dev->transfers_32bit = false;
    } else {
        if(memcmp(dev->identity, buf_32, 512) == 0){
            dev->transfers_32bit = true;
        } else {
            dev->transfers_32bit = false;
        }
    }

    dev->lba = BIT_IS_SET(dev->identity[49], 9);
    dev->dma = BIT_IS_SET(dev->identity[49], 8);
    dev->iordy = BIT_IS_SET(dev->identity[49], 11);
    dev->ext_func = BIT_IS_SET(dev->identity[83], 10);

    uint64_t total_sectors = ((uint64_t*)(dev->identity))[100];
    if(total_sectors == 0){
        total_sectors = ((uint32_t*)(dev->identity))[60];
        if(total_sectors == 0){
            total_sectors = dev->identity[1] * dev->identity[3] * dev->identity[6];
        }
    }
    dev->lba_max_sectors = total_sectors;

    if(dev->atapi){
        /*if(BIT_IS_SET(dev->identity[0], 0) && BIT_IS_CLEAR(dev->identity[0], 1)) dev->packet_bytes = 16;
        else if(BIT_IS_CLEAR(dev->identity[0], 0) && BIT_IS_CLEAR(dev->identity[0], 1)) dev->packet_bytes = 12;
        else {
            kprint("[ATA]: ATAPI device has unkown packet size\n");
            dev->exists = false;
            return false;
        }*/

        dev->packet_bytes = ((dev->identity[0] & 0x3) == 0) ? 12 : 16;
    }
    dev->exists = true;
    return true;
}

bool ata_check_identity(ata_device dev){
    uint8_t *p = (uint8_t*)dev.identity;
    uint8_t crc = 0;
    if(p[510] == 0xA5){
        for(uint32_t i = 0; i < 511; i++) crc += p[i];
        return ((uint8_t)(-crc) == p[511]);
    } else if(p[510] != 0){
        return false;
    } else {
        return true;
    }
}

bool ata_wait_busy(ata_device dev, uint32_t time){
    uint32_t timeout = time;
    while(timeout){
        if(!(inb(dev.cntrl_addr + ATA_ALT_STAT) & 0x80)){
            return true;
        }
        msleep(1);
        --timeout;
    }
    debug_log("[ATA]: ata_wait_busy timed out\n");

    inb(dev.cmd_addr + ATA_STATUS); // Clear interrupt status for the (probably) following command
    return false;
}

void ata_set_lba48(ata_device dev, uint64_t lba, uint16_t count){
    char buf[6];
    buf[0] = ((lba >> 0) & 0xFF);
    buf[1] = ((lba >> 8) & 0xFF);
    buf[2] = ((lba >> 16) & 0xFF);
    buf[3] = ((lba >> 24) & 0xFF);
    buf[4] = ((lba >> 32) & 0xFF);
    buf[5] = ((lba >> 40) & 0xFF);

    if(count == UINT16_MAX) count = 0;

    outb(dev.cmd_addr + ATA_SECT_CNT, ((count >> 8) & 0xFF));
    outb(dev.cmd_addr + ATA_LBA_LOW, buf[3]);
    outb(dev.cmd_addr + ATA_LBA_MID, buf[4]);
    outb(dev.cmd_addr + ATA_LBA_HIGH, buf[5]);
    outb(dev.cmd_addr + ATA_SECT_CNT, ((count >> 0) & 0xFF));
    outb(dev.cmd_addr + ATA_LBA_LOW, buf[0]);
    outb(dev.cmd_addr + ATA_LBA_MID, buf[1]);
    outb(dev.cmd_addr + ATA_LBA_HIGH, buf[2]);
}

bool ata_wait(ata_device dev, uint8_t bit, uint32_t timeout){
    ata_io_wait();

    while(timeout){
        uint8_t status = inb(dev.cntrl_addr + ATA_ALT_STAT);
        if(!(BIT_IS_SET(status, ATA_STATUS_BSY))){
            if(BIT_IS_SET(status, ATA_STATUS_ERR) || BIT_IS_SET(status, ATA_STATUS_DF)) return false;
            if(BIT_IS_SET(status, bit)) return true;
        }
        msleep(1);
        --timeout;
    }

    debug_log("[ATA]: ata_wait timed out\n");
    return false;
}

void ata_io_wait(){
    inb(ata_channels[0].cntrl_addr + ATA_ALT_STAT);
    inb(ata_channels[0].cntrl_addr + ATA_ALT_STAT);
    inb(ata_channels[0].cntrl_addr + ATA_ALT_STAT);
    inb(ata_channels[0].cntrl_addr + ATA_ALT_STAT);
}

bool ata_select_drv(ata_device dev, uint8_t flags, uint8_t lba24_head){
    uint8_t select = (flags & 0x40) ? ((0xA0 | 0x40 | (dev.slave << 4) | (lba24_head & 0x0F))) : ((0xA0 | 0x00 | (dev.slave << 4) | (lba24_head & 0x0F)));
    if(select == cur_selected) return true;

    uint8_t status = inb(dev.cmd_addr + ATA_STATUS);

    if(BIT_IS_SET(status, ATA_STATUS_BSY) || BIT_IS_SET(status, ATA_STATUS_DRQ)){
        kprint("[ATA]: Can't select drive\n");
        return false;
    }

    outb(dev.cmd_addr + ATA_DRV_HEAD, select);
    ata_io_wait();
    inb(dev.cmd_addr + ATA_STATUS);

    status = inb(dev.cmd_addr + ATA_STATUS);

    if(BIT_IS_SET(status, ATA_STATUS_BSY) || BIT_IS_SET(status, ATA_STATUS_DRQ)){
        kprint("[ATA]: Can't select drive\n");
        return false;
    }

    cur_selected = select;
    return true;
}

bool ata_identify_16(ata_device dev, uint16_t* buf, bool atapi){
    uint16_t* info = buf;
    ata_select_drv(dev, 0, 0);
    uint8_t cmd = (atapi) ? (ATA_COMMAND_IDENTIFY_PACKET_DEVICE) : (ATA_COMMAND_IDENTIFY);
    ata_wait_busy(dev, 100);
    outb(dev.cmd_addr + ATA_FEATURES, 0x0);
    outb(dev.cmd_addr + ATA_SECT_CNT, 0x0);
    outb(dev.cmd_addr + ATA_LBA_LOW, 0x0);
    outb(dev.cmd_addr + ATA_LBA_MID, 0x0);
    outb(dev.cmd_addr + ATA_LBA_HIGH, 0x0);
    outb(dev.cmd_addr + ATA_COMMAND, cmd);
    if(ata_wait_for_interrupt(dev, 100)){
        for(uint32_t i = 0; i < 256; i++) *info++ = inw(dev.cmd_addr + ATA_DATA);
        return true;
    }
    return false;
}

bool ata_identify_32(ata_device dev, uint16_t* buf, bool atapi){
    uint32_t* info = (uint32_t*)buf;
    ata_select_drv(dev, 0, 0);
    uint8_t cmd = (atapi) ? (ATA_COMMAND_IDENTIFY_PACKET_DEVICE) : (ATA_COMMAND_IDENTIFY);
    ata_wait_busy(dev, 100);
    outb(dev.cmd_addr + ATA_FEATURES, 0x0);
    outb(dev.cmd_addr + ATA_SECT_CNT, 0x0);
    outb(dev.cmd_addr + ATA_LBA_LOW, 0x0);
    outb(dev.cmd_addr + ATA_LBA_MID, 0x0);
    outb(dev.cmd_addr + ATA_LBA_HIGH, 0x0);
    outb(dev.cmd_addr + ATA_COMMAND, cmd);
    if(ata_wait_for_interrupt(dev, 100)){
        for(uint32_t i = 0; i < 128; i++) *info++ = ind(dev.cmd_addr + ATA_DATA);
        return true;
    }
    return false;
}

bool ata_read(ata_device dev, uint64_t start_sector, uint64_t sectors, void* buf){
    if(!dev.exists) return false;
    if(dev.lba_max_sectors < (start_sector + sectors)) return false;

    if(dev.dma && dev.bus_master_dma && dev.lba){
        if(dev.atapi){
            debug_log("[ATA]: Error unimplemented\n");
            return false;
        } else {
            uint8_t* b = (uint8_t*)buf;
            for(uint64_t i = 0; i < sectors; i++){
                if(ata_read_dma(dev, start_sector + i, b)){
                    b += ATA_SECTOR_SIZE;
                } else {
                    return false;
                }
            }
            return true;
        }
    } else {
        if(dev.transfers_32bit){
            return ata_read_pio_32(dev, start_sector, sectors, buf);
        } else {
            return ata_read_pio_16(dev, start_sector, sectors, buf);
        }
    }
}

bool ata_read_pio_32(ata_device dev, uint64_t start_sector, uint64_t sectors, void* buf){
    if(dev.ext_func){
        if(sectors > (UINT16_MAX + 1) || sectors == 0){
            kprint("[ATA]: To many sectors selected\n");
            return false;
        }
        uint32_t* data = (uint32_t*)buf;

        ata_select_drv(dev, (1<<6), 0);
        ata_wait_busy(dev, 100);

        ata_set_lba48(dev, start_sector, sectors);

        outb(dev.cmd_addr + ATA_COMMAND, ATA_COMMAND_READ_SECTORS_EXTENDED);

        if(ata_wait_for_interrupt(dev, 100)){
            while(sectors--){
               for(uint32_t i = 0; i < 128; i++) *data++ = ind(dev.cmd_addr + ATA_DATA);
               if(sectors && !ata_wait_for_interrupt(dev, 100)) return false;
            } 
            return true;
        }
        return false;
    } else {
        if(sectors > 256 || sectors == 0){
            kprint("[ATA]: To many sectors selected\n");
            return false;
        }
        if(dev.lba){
            uint32_t* data = (uint32_t*)buf;

            ata_select_drv(dev, (1<<6), (start_sector >> 24) & 0xF);
            ata_wait_busy(dev, 100);

            outb(dev.cmd_addr + ATA_FEATURES, 0x0);
            outb(dev.cmd_addr + ATA_SECT_CNT, (sectors == 256) ? (0) : (sectors));
            outb(dev.cmd_addr + ATA_LBA_LOW, (start_sector & 0xFF));
            outb(dev.cmd_addr + ATA_LBA_MID, (start_sector >> 8) & 0xFF);
            outb(dev.cmd_addr + ATA_LBA_HIGH, (start_sector >> 16) & 0xFF);
            outb(dev.cmd_addr + ATA_COMMAND, ATA_COMMAND_READ_SECTORS);

            if(ata_wait_for_interrupt(dev, 100)){
                while(sectors--){
                   for(uint32_t i = 0; i < 128; i++) *data++ = ind(dev.cmd_addr + ATA_DATA);
                   if(sectors && !ata_wait_for_interrupt(dev, 100)) return false;
                } 
              return true;
            }
            return false;
        } else {
            uint32_t* data = (uint32_t*)buf;

            ata_chs_t chs = lba_to_chs(dev, start_sector);
            ata_select_drv(dev, 0, chs.head);
            ata_wait_busy(dev, 100);

            outb(dev.cmd_addr + ATA_FEATURES, 0x0);
            outb(dev.cmd_addr + ATA_SECT_CNT, (sectors == 256) ? (0) : (sectors));
            outb(dev.cmd_addr + ATA_SECT_NUM, chs.sector & 0xFF);
            outb(dev.cmd_addr + ATA_CYL_LOW, (chs.cylinder & 0xFF));
            outb(dev.cmd_addr + ATA_CYL_HIGH, ((chs.cylinder >> 8) & 0xFF));
            outb(dev.cmd_addr + ATA_COMMAND, ATA_COMMAND_READ_SECTORS);

            if(ata_wait_for_interrupt(dev, 100)){
                while(sectors--){
                    for(uint32_t i = 0; i < 128; i++) *data++ = ind(dev.cmd_addr + ATA_DATA);
                    if(sectors && !ata_wait_for_interrupt(dev, 100)) return false;
                } 
                return true;
            }
            return false;
        }
        return false;
    }
}

bool ata_read_pio_16(ata_device dev, uint64_t start_sector, uint64_t sectors, void* buf){

    if(dev.ext_func){
        if(sectors > (UINT16_MAX + 1) || sectors == 0){
            kprint("[ATA]: To many sectors selected\n");
            return false;
        }
        uint16_t* data = buf;

        ata_select_drv(dev, (1<<6), 0);
        ata_wait_busy(dev, 100);

        ata_set_lba48(dev, start_sector, sectors);

        outb(dev.cmd_addr + ATA_COMMAND, ATA_COMMAND_READ_SECTORS_EXTENDED);

        if(ata_wait_for_interrupt(dev, 100)){
            while(sectors--){
               for(uint32_t i = 0; i < 256; i++) *data++ = inw(dev.cmd_addr + ATA_DATA);
               if(sectors && !ata_wait_for_interrupt(dev, 100)) return false;
            } 
            return true;
        }
        return false;
    } else {
        if(sectors > 256 || sectors == 0){
            kprint("[ATA]: To many sectors selected\n");
            return false;
        }
        if(dev.lba){
            uint16_t* data = buf;

            ata_select_drv(dev, (1<<6), (start_sector >> 24) & 0xF);
            ata_wait_busy(dev, 100);

            outb(dev.cmd_addr + ATA_FEATURES, 0x0);
            outb(dev.cmd_addr + ATA_SECT_CNT, (sectors == 256) ? (0) : (sectors));
            outb(dev.cmd_addr + ATA_LBA_LOW, (start_sector & 0xFF));
            outb(dev.cmd_addr + ATA_LBA_MID, (start_sector >> 8) & 0xFF);
            outb(dev.cmd_addr + ATA_LBA_HIGH, (start_sector >> 16) & 0xFF);
            outb(dev.cmd_addr + ATA_COMMAND, ATA_COMMAND_READ_SECTORS);

            if(ata_wait_for_interrupt(dev, 100)){
                while(sectors--){
                   for(uint32_t i = 0; i < 256; i++) *data++ = inw(dev.cmd_addr + ATA_DATA);
                   if(sectors && !ata_wait_for_interrupt(dev, 100)) return false;
                } 
              return true;
            }
            return false;
        } else {
            uint16_t* data = buf;

            ata_chs_t chs = lba_to_chs(dev, start_sector);
            ata_select_drv(dev, 0, chs.head);
            ata_wait_busy(dev, 100);

            outb(dev.cmd_addr + ATA_FEATURES, 0x0);
            outb(dev.cmd_addr + ATA_SECT_CNT, (sectors == 256) ? (0) : (sectors));
            outb(dev.cmd_addr + ATA_SECT_NUM, chs.sector & 0xFF);
            outb(dev.cmd_addr + ATA_CYL_LOW, (chs.cylinder & 0xFF));
            outb(dev.cmd_addr + ATA_CYL_HIGH, ((chs.cylinder >> 8) & 0xFF));
            outb(dev.cmd_addr + ATA_COMMAND, ATA_COMMAND_READ_SECTORS);

            if(ata_wait_for_interrupt(dev, 100)){
                while(sectors--){
                    for(uint32_t i = 0; i < 256; i++) *data++ = inw(dev.cmd_addr + ATA_DATA);
                    if(sectors && !ata_wait_for_interrupt(dev, 100)) return false;
                } 
                return true;
            }
            return false;
        }
        return false;
    }
}

bool ata_write(ata_device dev, uint64_t start_sector, uint64_t sectors, void* buf){
    if(!dev.exists) return false;
    if(dev.lba_max_sectors < (start_sector + sectors)) return false;

    if(dev.dma && dev.bus_master_dma && dev.lba){
        if(dev.atapi){
            debug_log("[ATA]: Error unimplemented\n");
            return false;
        } else {
            uint8_t* b = (uint8_t*)buf;
            for(uint64_t i = 0; i < sectors; i++){
                if(ata_write_dma(dev, start_sector + i, b)){
                    b += ATA_SECTOR_SIZE;
                } else {
                    return false;
                }
            }
            return true;
        }
    } else {
        if(dev.transfers_32bit){
            return ata_write_pio_32(dev, start_sector, sectors, buf);
        } else {
            return ata_write_pio_16(dev, start_sector, sectors, buf);
        }
    }
}

bool ata_write_pio_32(ata_device dev, uint64_t start_sector, uint64_t sectors, void* buf){
    if(dev.ext_func){
        if(sectors > (UINT16_MAX + 1) || sectors == 0){
            kprint("[ATA]: To many sectors selected\n");
            return false;
        }
        uint32_t* data = (uint32_t*)buf;

        ata_select_drv(dev, (1<<6), 0);
        ata_wait_busy(dev, 100);

        ata_set_lba48(dev, start_sector, sectors);

        outb(dev.cmd_addr + ATA_COMMAND, ATA_COMMAND_WRITE_SECTORS_EXTENDED);

        if(ata_wait_for_interrupt(dev, 100)){
            while(sectors--){
               for(uint32_t i = 0; i < 128; i++) outd(dev.cmd_addr + ATA_DATA, *data++);
               if(sectors && !ata_wait_for_interrupt(dev, 100)) return false;
            } 
            return true;
        }
        return false;
    } else {
        if(sectors > 256 || sectors == 0){
            kprint("[ATA]: To many sectors selected\n");
            return false;
        }
        if(dev.lba){
            uint32_t* data = (uint32_t*)buf;

            ata_select_drv(dev, (1<<6), (start_sector >> 24) & 0xF);
            ata_wait_busy(dev, 100);

            outb(dev.cmd_addr + ATA_FEATURES, 0x0);
            outb(dev.cmd_addr + ATA_SECT_CNT, (sectors == 256) ? (0) : (sectors));
            outb(dev.cmd_addr + ATA_LBA_LOW, (start_sector & 0xFF));
            outb(dev.cmd_addr + ATA_LBA_MID, (start_sector >> 8) & 0xFF);
            outb(dev.cmd_addr + ATA_LBA_HIGH, (start_sector >> 16) & 0xFF);
            outb(dev.cmd_addr + ATA_COMMAND, ATA_COMMAND_WRITE_SECTORS);

            if(ata_wait_for_interrupt(dev, 100)){
                while(sectors--){
                   for(uint32_t i = 0; i < 128; i++) outd(dev.cmd_addr + ATA_DATA, *data++);
                   if(sectors && !ata_wait_for_interrupt(dev, 100)) return false;
                } 
              return true;
            }
            return false;
        } else {
            uint32_t* data = (uint32_t*)buf;

            ata_chs_t chs = lba_to_chs(dev, start_sector);
            ata_select_drv(dev, 0, chs.head);
            ata_wait_busy(dev, 100);

            outb(dev.cmd_addr + ATA_FEATURES, 0x0);
            outb(dev.cmd_addr + ATA_SECT_CNT, (sectors == 256) ? (0) : (sectors));
            outb(dev.cmd_addr + ATA_SECT_NUM, chs.sector & 0xFF);
            outb(dev.cmd_addr + ATA_CYL_LOW, (chs.cylinder & 0xFF));
            outb(dev.cmd_addr + ATA_CYL_HIGH, ((chs.cylinder >> 8) & 0xFF));
            outb(dev.cmd_addr + ATA_COMMAND, ATA_COMMAND_WRITE_SECTORS);

            if(ata_wait_for_interrupt(dev, 100)){
                while(sectors--){
                    for(uint32_t i = 0; i < 128; i++) outd(dev.cmd_addr + ATA_DATA, *data++);
                    if(sectors && !ata_wait_for_interrupt(dev, 100)) return false;
                } 
                return true;
            }
            return false;
        }
    }

}

bool ata_write_pio_16(ata_device dev, uint64_t start_sector, uint64_t sectors, void* buf){
    if(dev.ext_func){
        if(sectors > (UINT16_MAX + 1) || sectors == 0){
            kprint("[ATA]: To many sectors selected\n");
            return false;
        }
        uint16_t* data = buf;

        ata_select_drv(dev, (1<<6), 0);
        ata_wait_busy(dev, 100);

        ata_set_lba48(dev, start_sector, sectors);

        outb(dev.cmd_addr + ATA_COMMAND, ATA_COMMAND_WRITE_SECTORS_EXTENDED);

        if(ata_wait_for_interrupt(dev, 100)){
            while(sectors--){
               for(uint32_t i = 0; i < 256; i++) outw(dev.cmd_addr + ATA_DATA, *data++);
               if(sectors && !ata_wait_for_interrupt(dev, 100)) return false;
            } 
            return true;
        }
        return false;
    } else {
        if(sectors > 256 || sectors == 0){
            kprint("[ATA]: To many sectors selected\n");
            return false;
        }
        if(dev.lba){
            uint16_t* data = buf;

            ata_select_drv(dev, (1<<6), (start_sector >> 24) & 0xF);
            ata_wait_busy(dev, 100);

            outb(dev.cmd_addr + ATA_FEATURES, 0x0);
            outb(dev.cmd_addr + ATA_SECT_CNT, (sectors == 256) ? (0) : (sectors));
            outb(dev.cmd_addr + ATA_LBA_LOW, (start_sector & 0xFF));
            outb(dev.cmd_addr + ATA_LBA_MID, (start_sector >> 8) & 0xFF);
            outb(dev.cmd_addr + ATA_LBA_HIGH, (start_sector >> 16) & 0xFF);
            outb(dev.cmd_addr + ATA_COMMAND, ATA_COMMAND_WRITE_SECTORS);

            if(ata_wait_for_interrupt(dev, 100)){
                while(sectors--){
                   for(uint32_t i = 0; i < 256; i++) outw(dev.cmd_addr + ATA_DATA, *data++);
                   if(sectors && !ata_wait_for_interrupt(dev, 100)) return false;
                } 
              return true;
            }
            return false;
        } else {
            uint16_t* data = buf;

            ata_chs_t chs = lba_to_chs(dev, start_sector);
            ata_select_drv(dev, 0, chs.head);
            ata_wait_busy(dev, 100);

            outb(dev.cmd_addr + ATA_FEATURES, 0x0);
            outb(dev.cmd_addr + ATA_SECT_CNT, (sectors == 256) ? (0) : (sectors));
            outb(dev.cmd_addr + ATA_SECT_NUM, chs.sector & 0xFF);
            outb(dev.cmd_addr + ATA_CYL_LOW, (chs.cylinder & 0xFF));
            outb(dev.cmd_addr + ATA_CYL_HIGH, ((chs.cylinder >> 8) & 0xFF));
            outb(dev.cmd_addr + ATA_COMMAND, ATA_COMMAND_WRITE_SECTORS);

            if(ata_wait_for_interrupt(dev, 100)){
                while(sectors--){
                    for(uint32_t i = 0; i < 256; i++) outw(dev.cmd_addr + ATA_DATA, *data++);
                    if(sectors && !ata_wait_for_interrupt(dev, 100)) return false;
                } 
                return true;
            }
            return false;
        }
    }

}

bool atapi_send_packet(ata_device dev, uint8_t* packet, uint8_t* return_buffer, uint64_t return_buffer_len){
    if(!dev.atapi){
        debug_log("[ATA] Tried to do ATAPI operation with non ATAPI device\n");
        kprint("[ATA] Tried to do ATAPI operation with non ATAPI device\n");
        return false;
    }
    uint16_t* ptr = (uint16_t*)return_buffer;

    if(!ata_wait_busy(dev, 100)){
        debug_log("[ATA]: ata_wait_busy timed out in atapi_send_packet");
        return false;
    }

    ata_select_drv(dev, 0, 0);

    ata_clear_interrupt_status(dev);

    outb(dev.cmd_addr + ATA_FEATURES, 0x0);
    outb(dev.cmd_addr + ATA_SECT_CNT, 0);
    outb(dev.cmd_addr + ATA_LBA_LOW, 0);
    outb(dev.cmd_addr + ATA_LBA_MID, (return_buffer_len >> 0) & 0xFF);
    outb(dev.cmd_addr + ATA_LBA_HIGH, (return_buffer_len >> 8) & 0xFF);
    outb(dev.cmd_addr + ATA_COMMAND, ATA_COMMAND_SEND_PACKET);
    if(ata_wait_for_interrupt(dev, 100)){//ata_wait(dev, ATA_STATUS_DRQ, 100)){
        for(uint32_t i = 0; i < dev.packet_bytes; i += 2) outw(dev.cmd_addr + ATA_DATA, packet[i] | (packet[i+1] << 8));

        if(ata_wait_for_interrupt(dev, 100)){
            uint16_t transfer_bytes = inb(dev.cmd_addr + ATA_LBA_MID) | (inb(dev.cmd_addr + ATA_LBA_HIGH) << 8);
            for(uint32_t i = 0; i < (transfer_bytes / 2); i++){
                *ptr++ = inw(dev.cmd_addr + ATA_DATA);
            }
            return true;
        }
    }
    return false;
}

bool ata_init_dma(ata_device dev, void* addr, uint32_t sector_size, uint8_t direction){
    if(!dev.bus_master_dma){
        debug_log("[ATA]: Selected device does not have a PCI bus master DMA\n");
        return false;
    }

    prdt_t* prdt = (dev.secondary) ? (&ata_secondary_prdt) : (&ata_secondary_prdt);
    uint32_t prdt_phys = (uint32_t)vmm_virt_to_phys((void*)prdt);
    uint32_t page_phys = (uint32_t)vmm_virt_to_phys(addr);

    prdt->physical_base = page_phys;
    prdt->transfer_size = sector_size;
    prdt->end_of_transfer = 1;

    if(dev.secondary){
        uint8_t reg = inb(dev.bus_master_addr + BM1_COMMAND);
        BIT_CLEAR(reg, BM_COMMAND_START);
        reg = (direction << BM_COMMAND_DIRECTION);
        outb(dev.bus_master_addr + BM1_COMMAND, reg);
        outd(dev.bus_master_addr + BM1_ADDRESS, prdt_phys);
        outb(dev.bus_master_addr + BM1_STATUS, 0x6); // 00000110b Clear bit 1 and 2
    } else {
        uint8_t reg = inb(dev.bus_master_addr + BM0_COMMAND);
        BIT_CLEAR(reg, BM_COMMAND_START);
        reg = (direction << BM_COMMAND_DIRECTION);
        outb(dev.bus_master_addr + BM0_COMMAND, reg);
        outd(dev.bus_master_addr + BM0_ADDRESS, prdt_phys);
        outb(dev.bus_master_addr + BM0_STATUS, 0x6); // 00000110b Clear bit 1 and 2
    }
    return true;
}

bool ata_start_dma(ata_device dev){
    if(!dev.bus_master_dma){
        debug_log("[ATA]: Selected device does not have a PCI bus master DMA\n");
        return false;
    }

    if(dev.secondary){
        uint8_t reg = inb(dev.bus_master_addr + BM1_COMMAND);
        BIT_SET(reg, BM_COMMAND_START);
        outb(dev.bus_master_addr + BM1_COMMAND, reg);
    } else {
        uint8_t reg = inb(dev.bus_master_addr + BM0_COMMAND);
        BIT_SET(reg, BM_COMMAND_START);
        outb(dev.bus_master_addr + BM0_COMMAND, reg);
    }
    return true;
}


bool ata_stop_dma(ata_device dev){
    if(!dev.bus_master_dma){
        debug_log("[ATA]: Selected device does not have a PCI bus master DMA\n");
        return false;
    }

    if(dev.secondary){
        uint8_t reg = inb(dev.bus_master_addr + BM1_COMMAND);
        BIT_CLEAR(reg, BM_COMMAND_START);
        outb(dev.bus_master_addr + BM1_COMMAND, reg);
    } else {
        uint8_t reg = inb(dev.bus_master_addr + BM0_COMMAND);
        BIT_CLEAR(reg, BM_COMMAND_START);
        outb(dev.bus_master_addr + BM0_COMMAND, reg);
    }
    return true;
}

bool ata_read_dma(ata_device dev, uint64_t lba, uint8_t* buf){
    if(dev.atapi){
        debug_log("[ATA]: Tried to execute ATA DMA operation with ATAPI device\n");
        return false;
    }

    uint8_t dma_buf[ATA_SECTOR_SIZE] __attribute__ ((aligned (2)));
    memset(dma_buf, 0, ATA_SECTOR_SIZE);

    ata_init_dma(dev, (void*)dma_buf, ATA_SECTOR_SIZE, DMA_DIRECTION_READ);

    if(dev.ext_func){
        ata_select_drv(dev, (1<<6), 0);
        ata_wait_busy(dev, 100);

        ata_set_lba48(dev, lba, 1);
        ata_clear_interrupt_status(dev);
        outb(dev.cmd_addr + ATA_COMMAND, ATA_COMMAND_READ_SECTORS_EXTENDED_DMA);
        ata_start_dma(dev);

        if(ata_wait_for_interrupt(dev, 1000)){
            ata_stop_dma(dev);
            /*uint8_t ata_status = */inb(dev.cmd_addr + ATA_STATUS);
            uint8_t off = (dev.secondary) ? (BM1_STATUS) : (BM0_STATUS);
            uint8_t bm_status = inb(dev.bus_master_addr + off);
            if(BIT_IS_SET(bm_status, BM_STATUS_ERROR)){
                debug_log("[ATA]: Bus master Error\n");
                return false;
            } else if (BIT_IS_SET(bm_status, BM_STATUS_INTERRUPT) && BIT_IS_CLEAR(bm_status, BM_STATUS_ACTIVE)){
                memcpy(buf, dma_buf, ATA_SECTOR_SIZE);
                return true;
            }
            debug_log("[ATA]: DMA Unkown Error\n");
            return false;
        } else {
            ata_stop_dma(dev);
            debug_log("[ATA]: No DMA Interrupt received\n");
            return false;
        }
    } else {
        if(dev.lba){
            ata_select_drv(dev, (1<<6), (lba >> 24) & 0xF);
            ata_wait_busy(dev, 100);

            outb(dev.cmd_addr + ATA_FEATURES, 0x0);
            outb(dev.cmd_addr + ATA_SECT_CNT, 1);
            outb(dev.cmd_addr + ATA_LBA_LOW, (lba & 0xFF));
            outb(dev.cmd_addr + ATA_LBA_MID, (lba >> 8) & 0xFF);
            outb(dev.cmd_addr + ATA_LBA_HIGH, (lba >> 16) & 0xFF);

            ata_clear_interrupt_status(dev);
            outb(dev.cmd_addr + ATA_COMMAND, ATA_COMMAND_READ_SECTORS_DMA);
            ata_start_dma(dev);
            if(ata_wait_for_interrupt(dev, 1000)){
                ata_stop_dma(dev);
                /*uint8_t ata_status = */inb(dev.cmd_addr + ATA_STATUS);
                uint8_t off = (dev.secondary) ? (BM1_STATUS) : (BM0_STATUS);
                uint8_t bm_status = inb(dev.bus_master_addr + off);
                if(BIT_IS_SET(bm_status, BM_STATUS_ERROR)){
                    debug_log("[ATA]: Bus master Error\n");
                    return false;
                } else if (BIT_IS_SET(bm_status, BM_STATUS_INTERRUPT) && BIT_IS_CLEAR(bm_status, BM_STATUS_ACTIVE)){
                    memcpy(buf, dma_buf, ATA_SECTOR_SIZE);
                    return true;
                }
                debug_log("[ATA]: DMA Unkown Error\n");
                return false;
            } else {
                ata_stop_dma(dev);
                debug_log("[ATA]: No DMA Interrupt received\n");
                return false;
            }
        } else {
            debug_log("[ATA]: No CHS DMA supported\n");
            return false;
        }
    }
    return false;
}

bool ata_write_dma(ata_device dev, uint64_t lba, uint8_t* buf){
    if(dev.atapi){
        debug_log("[ATA]: Tried to execute ATA DMA operation with ATAPI device\n");
        return false;
    }

    uint8_t dma_buf[ATA_SECTOR_SIZE] __attribute__ ((aligned (2)));
    memcpy(dma_buf, buf, 512);

    ata_init_dma(dev, (void*)dma_buf, ATA_SECTOR_SIZE, DMA_DIRECTION_WRITE);

    if(dev.ext_func){
        ata_select_drv(dev, (1<<6), 0);
        ata_wait_busy(dev, 100);

        ata_set_lba48(dev, lba, 1);
        ata_clear_interrupt_status(dev);
        outb(dev.cmd_addr + ATA_COMMAND, ATA_COMMAND_WRITE_SECTORS_EXTENDED_DMA);
        ata_start_dma(dev);

        if(ata_wait_for_interrupt(dev, 1000)){
            ata_stop_dma(dev);
            /*uint8_t ata_status = */inb(dev.cmd_addr + ATA_STATUS);
            uint8_t off = (dev.secondary) ? (BM1_STATUS) : (BM0_STATUS);
            uint8_t bm_status = inb(dev.bus_master_addr + off);
            if(BIT_IS_SET(bm_status, BM_STATUS_ERROR)){
                debug_log("[ATA]: Bus master Error\n");
                return false;
            } else if (BIT_IS_SET(bm_status, BM_STATUS_INTERRUPT) && BIT_IS_CLEAR(bm_status, BM_STATUS_ACTIVE)){
                return true;
            }
            debug_log("[ATA]: DMA Unkown Error\n");
            return false;
        } else {
            ata_stop_dma(dev);
            debug_log("[ATA]: No DMA Interrupt received\n");
            return false;
        }
    } else {
        if(dev.lba){
            ata_select_drv(dev, (1<<6), (lba >> 24) & 0xF);
            ata_wait_busy(dev, 100);

            outb(dev.cmd_addr + ATA_FEATURES, 0x0);
            outb(dev.cmd_addr + ATA_SECT_CNT, 1);
            outb(dev.cmd_addr + ATA_LBA_LOW, (lba & 0xFF));
            outb(dev.cmd_addr + ATA_LBA_MID, (lba >> 8) & 0xFF);
            outb(dev.cmd_addr + ATA_LBA_HIGH, (lba >> 16) & 0xFF);

            ata_clear_interrupt_status(dev);
            outb(dev.cmd_addr + ATA_COMMAND, ATA_COMMAND_WRITE_SECTORS_DMA);
            ata_start_dma(dev);
            if(ata_wait_for_interrupt(dev, 1000)){
                ata_stop_dma(dev);
                /*uint8_t ata_status = */inb(dev.cmd_addr + ATA_STATUS);
                uint8_t off = (dev.secondary) ? (BM1_STATUS) : (BM0_STATUS);
                uint8_t bm_status = inb(dev.bus_master_addr + off);
                if(BIT_IS_SET(bm_status, BM_STATUS_ERROR)){
                    debug_log("[ATA]: Bus master Error\n");
                    return false;
                } else if (BIT_IS_SET(bm_status, BM_STATUS_INTERRUPT) && BIT_IS_CLEAR(bm_status, BM_STATUS_ACTIVE)){
                    return true;
                }
                debug_log("[ATA]: DMA Unkown Error\n");
                return false;
            } else {
                ata_stop_dma(dev);
                debug_log("[ATA]: No DMA Interrupt received\n");
                return false;
            }
        } else {
            debug_log("[ATA]: No CHS DMA supported\n");
            return false;
        }
    }
    return false;
}
