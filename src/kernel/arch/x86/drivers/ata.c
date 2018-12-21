#include <rhino/arch/x86/drivers/ata.h>

ata_device primary_master, primary_slave;

uint8_t cur_selected = 0xFF;

static ata_chs_t lba_to_chs(ata_device dev, uint64_t lba){
    ata_chs_t chs;
    uint32_t hpc, spt;
    hpc = dev.identitiy[3];
    spt = dev.identitiy[6];

    chs.cylinder = lba / (hpc * spt);
    chs.head = (lba / spt) % hpc;
    chs.sector = (lba % spt) + 1;
    return chs;
}

void ata_init(uint16_t bus, uint8_t device, uint8_t function){
    if(bus > PCI_BUS_N || device > PCI_DEVICE_N || function > PCI_FUNCTION_N){
        kprint("[ATA]: PCI Parameters invalid\n");
        return;
    }

    primary_master.cmd_addr = 0x1f0;
    primary_master.cntrl_addr = 0x3f4;
    primary_master.slave = false;

    primary_slave.cmd_addr = 0x1f0;
    primary_slave.cntrl_addr = 0x3f4;
    primary_slave.slave = true;

    outb(primary_master.cmd_addr + ATA_SECT_CNT, 0x2A);
    outb(primary_master.cmd_addr + ATA_SECT_NUM, 0x55);

    msleep(15);

    uint8_t a = inb(primary_master.cmd_addr + ATA_SECT_CNT);
    uint8_t b = inb(primary_master.cmd_addr + ATA_SECT_NUM);

    if((a != 0x2A) && (b == 0x55)){
        kprint("[ATA]: No ATA controller?\n");
        return;
    }

    outb(primary_master.cmd_addr + ATA_DRV_HEAD, 0);
    ata_io_wait();
    outb(primary_master.cntrl_addr + ATA_DEV_CNTR, (1<<2));
    msleep(1);
    outb(primary_master.cntrl_addr + ATA_DEV_CNTR, (1<<1));
    msleep(2);
    ata_wait_busy(primary_master, 100);
    msleep(5);
    /*if(inb(primary_master.cmd_addr + ATA_ERROR) != 0){
        kprint("[ATA]: Error not 0 after reset");
        return;
    }*/

    primary_master.id_high = inb(primary_master.cmd_addr + ATA_LBA_HIGH);
    primary_master.id_mid = inb(primary_master.cmd_addr + ATA_LBA_MID);

    if((primary_master.id_high == 0x00) && (primary_master.id_mid == 0x00)) primary_master.atapi = false;
    else if((primary_master.id_high == 0xEB) && (primary_master.id_mid == 0x14)) primary_master.atapi = true;

    ata_identify(primary_master, primary_master.identitiy, primary_master.atapi);

    if(!ata_check_identity(primary_master)){
        kprint("[ATA]: Info block invalid");
        return;
    }

    primary_master.lba = BIT_IS_SET(primary_master.identitiy[49], 9);
    primary_master.dma = BIT_IS_SET(primary_master.identitiy[49], 8);
    primary_master.iordy = BIT_IS_SET(primary_master.identitiy[49], 11);
    primary_master.ext_func = BIT_IS_SET(primary_master.identitiy[83], 10);
}

bool ata_check_identity(ata_device dev){
    uint8_t *p = (uint8_t*)dev.identitiy;
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
    kprint("[ATA]: ata_wait_busy timed out\n");
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

    kprint("[ATA]: ata_wait timed out\n");
    return false;
}

void ata_io_wait(){
    inb(primary_master.cntrl_addr + ATA_ALT_STAT);
    inb(primary_master.cntrl_addr + ATA_ALT_STAT);
    inb(primary_master.cntrl_addr + ATA_ALT_STAT);
    inb(primary_master.cntrl_addr + ATA_ALT_STAT);
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

bool ata_identify(ata_device dev, uint16_t* buf, bool atapi){
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
    if(ata_wait(dev, ATA_STATUS_DRQ, 100)){
        for(uint32_t i = 0; i < 256; i++) *info++ = inw(dev.cmd_addr + ATA_DATA);
        return true;
    }
    return false;
}


bool ata_read(ata_device dev, uint64_t start_sector, uint64_t sectors, void* buf){
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

        if(ata_wait(dev, ATA_STATUS_DRQ, 100)){
            while(sectors--){
               for(uint32_t i = 0; i < 256; i++) *data++ = inw(dev.cmd_addr + ATA_DATA);
               if(sectors && !ata_wait(dev, ATA_STATUS_DRQ, 100)) return false;
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

            ata_select_drv(dev, (1<<6), (start_sector << 24) & 0xF);
            ata_wait_busy(dev, 100);

            outb(dev.cmd_addr + ATA_FEATURES, 0x0);
            outb(dev.cmd_addr + ATA_SECT_CNT, (sectors == 256) ? (0) : (sectors));
            outb(dev.cmd_addr + ATA_LBA_LOW, (start_sector & 0xFF));
            outb(dev.cmd_addr + ATA_LBA_MID, (start_sector << 8) & 0xFF);
            outb(dev.cmd_addr + ATA_LBA_HIGH, (start_sector << 16) & 0xFF);
            outb(dev.cmd_addr + ATA_COMMAND, ATA_COMMAND_READ_SECTORS);

            if(ata_wait(dev, ATA_STATUS_DRQ, 100)){
                while(sectors--){
                   for(uint32_t i = 0; i < 256; i++) *data++ = inw(dev.cmd_addr + ATA_DATA);
                   if(sectors && !ata_wait(dev, ATA_STATUS_DRQ, 100)) return false;
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
            outb(dev.cmd_addr + ATA_CYL_HIGH, ((chs.cylinder << 8) & 0xFF));
            outb(dev.cmd_addr + ATA_COMMAND, ATA_COMMAND_READ_SECTORS);

            if(ata_wait(dev, ATA_STATUS_DRQ, 100)){
                while(sectors--){
                    for(uint32_t i = 0; i < 256; i++) *data++ = inw(dev.cmd_addr + ATA_DATA);
                    if(sectors && !ata_wait(dev, ATA_STATUS_DRQ, 100)) return false;
                } 
                return true;
            }
            return false;
        }
        return false;
    }

}

bool ata_write(ata_device dev, uint64_t start_sector, uint64_t sectors, void* buf){
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

        if(ata_wait(dev, ATA_STATUS_DRQ, 100)){
            while(sectors--){
               for(uint32_t i = 0; i < 256; i++) outw(dev.cmd_addr + ATA_DATA, *data++);
               if(sectors && !ata_wait(dev, ATA_STATUS_DRQ, 100)) return false;
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

            ata_select_drv(dev, (1<<6), (start_sector << 24) & 0xF);
            ata_wait_busy(dev, 100);

            outb(dev.cmd_addr + ATA_FEATURES, 0x0);
            outb(dev.cmd_addr + ATA_SECT_CNT, (sectors == 256) ? (0) : (sectors));
            outb(dev.cmd_addr + ATA_LBA_LOW, (start_sector & 0xFF));
            outb(dev.cmd_addr + ATA_LBA_MID, (start_sector << 8) & 0xFF);
            outb(dev.cmd_addr + ATA_LBA_HIGH, (start_sector << 16) & 0xFF);
            outb(dev.cmd_addr + ATA_COMMAND, ATA_COMMAND_WRITE_SECTORS);

            if(ata_wait(dev, ATA_STATUS_DRQ, 100)){
                while(sectors--){
                   for(uint32_t i = 0; i < 256; i++) outw(dev.cmd_addr + ATA_DATA, *data++);
                   if(sectors && !ata_wait(dev, ATA_STATUS_DRQ, 100)) return false;
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
            outb(dev.cmd_addr + ATA_CYL_HIGH, ((chs.cylinder << 8) & 0xFF));
            outb(dev.cmd_addr + ATA_COMMAND, ATA_COMMAND_WRITE_SECTORS);

            if(ata_wait(dev, ATA_STATUS_DRQ, 100)){
                while(sectors--){
                    for(uint32_t i = 0; i < 256; i++) outw(dev.cmd_addr + ATA_DATA, *data++);
                    if(sectors && !ata_wait(dev, ATA_STATUS_DRQ, 100)) return false;
                } 
                return true;
            }
            return false;
        }
        return false;
    }

}