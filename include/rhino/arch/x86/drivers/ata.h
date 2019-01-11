#pragma once

#include <rhino/common.h>
#include <rhino/kernel.h>
#include <rhino/arch/x86/pci.h>
#include <rhino/arch/x86/isr.h>
#include <rhino/arch/x86/io.h>
#include <rhino/arch/x86/timer.h>
#include <rhino/arch/x86/drivers/serial.h>
#include <rhino/multitasking/spinlock.h>
#include <rhino/mm/hmm.h>

#define ATA_DATA 0x0
#define ATA_ERROR 0x1
#define ATA_FEATURES 0x1
#define ATA_SECT_CNT 0x2
#define ATA_SECT_NUM 0x3
#define ATA_LBA_LOW 0x3
#define ATA_CYL_LOW 0x4
#define ATA_LBA_MID 0x4
#define ATA_CYL_HIGH 0x5
#define ATA_LBA_HIGH 0x5
#define ATA_DRV_HEAD 0x6
#define ATA_STATUS 0x7
#define ATA_COMMAND 0x7

#define ATA_ALT_STAT 0x2
#define ATA_DEV_CNTR 0x2
#define ATA_DRV_ADDR 0x3

#define BM0_COMMAND 0x0
#define BM0_RESV0 0x1
#define BM0_STATUS 0x2
#define BM0_RESV1 0x3
#define BM0_ADDRESS 0x4

#define BM1_COMMAND 0x8
#define BM1_RESV0 0x9
#define BM1_STATUS 0xA
#define BM1_RESV1 0xB
#define BM1_ADDRESS 0xC

#define BM_COMMAND_START 0
#define BM_COMMAND_DIRECTION 3

#define DMA_DIRECTION_WRITE 0
#define DMA_DIRECTION_READ 1

#define BM_STATUS_SIMPLEX 7
#define BM_STATUS_DRV1_DMA 6
#define BM_STATUS_DRV2_DMA 5
#define BM_STATUS_INTERRUPT 2
#define BM_STATUS_ERROR 1
#define BM_STATUS_ACTIVE 0

#define ATA_STATUS_ERR 0
#define ATA_STATUS_DRQ 3
#define ATA_STATUS_DF 5
#define ATA_STATUS_RDY 6
#define ATA_STATUS_BSY 7

#define ATA_DRV_HEAD_DRV_SELECT 4
#define ATA_DRV_HEAD_LBA_MODE 6

#define ATA_DEV_CONTROL_SRST 2
#define ATA_DEV_CONTROL_NIEN 1

#define ATA_COMMAND_IDENTIFY 0xEC
#define ATA_COMMAND_IDENTIFY_PACKET_DEVICE 0xA1

#define ATA_COMMAND_READ_SECTORS 0x20
#define ATA_COMMAND_READ_SECTORS_DMA 0xC8
#define ATA_COMMAND_READ_SECTORS_EXTENDED 0x24
#define ATA_COMMAND_READ_SECTORS_EXTENDED_DMA 0x25

#define ATA_COMMAND_WRITE_SECTORS 0x30
#define ATA_COMMAND_WRITE_SECTORS_DMA 0xCA
#define ATA_COMMAND_WRITE_SECTORS_EXTENDED 0x34
#define ATA_COMMAND_WRITE_SECTORS_EXTENDED_DMA 0x35

#define ATA_COMMAND_SEND_PACKET 0xA0

#define ATAPI_COMMAND_READ_CAPACITY 0x25

// Most devices will have these it is not foolproof but good enough
#define ATA_SECTOR_SIZE 512
#define ATAPI_SECTOR_SIZE 2048

typedef struct {
    uint32_t physical_base; // 4byte aligned base addr bit0 should stay clear
    uint16_t transfer_size; // in bytes, 0 is UINT16_MAX
    uint16_t reserved : 15;
    uint8_t end_of_transfer : 1;
} __attribute__((packed)) prdt_t;

typedef struct{
    uint16_t cmd_addr, cntrl_addr, bus_master_addr;
    uint8_t id_high, id_mid, packet_bytes, channel_num;
    uint16_t identity[256];
    bool lba, dma, iordy, ext_func, exists, atapi, slave, secondary, transfers_32bit, bus_master_dma;
    uint64_t lba_max_sectors;
} ata_device;

typedef struct {
    uint32_t cylinder, head, sector;
} ata_chs_t;

void ata_init(uint16_t bus, uint8_t device, uint8_t function);

bool ata_read(ata_device dev, uint64_t start_sector, uint64_t sectors, void* buf);
bool ata_write(ata_device dev, uint64_t start_sector, uint64_t sectors, void* buf);

bool ata_init_device(ata_device* dev);

bool ata_identify_16(ata_device dev, uint16_t* buf, bool atapi);
bool ata_identify_32(ata_device dev, uint16_t* buf, bool atapi);
bool ata_check_identity(ata_device dev);

bool ata_read_pio_16(ata_device dev, uint64_t start_sector, uint64_t sectors, void* buf);
bool ata_read_pio_32(ata_device dev, uint64_t start_sector, uint64_t sectors, void* buf);
bool ata_write_pio_16(ata_device dev, uint64_t start_sector, uint64_t sectors, void* buf);
bool ata_write_pio_32(ata_device dev, uint64_t start_sector, uint64_t sectors, void* buf);

bool ata_read_dma(ata_device dev, uint64_t lba, uint8_t* buf);
bool ata_write_dma(ata_device dev, uint64_t lba, uint8_t* buf);


bool atapi_send_packet(ata_device dev, uint8_t* packet, uint8_t* return_buffer, uint64_t return_buffer_len);

bool ata_init_dma(ata_device dev, void* addr, uint32_t sector_size, uint8_t direction);
bool ata_start_dma(ata_device dev);
bool ata_stop_dma(ata_device dev);

void ata_set_lba48(ata_device dev, uint64_t lba, uint16_t count);
bool ata_wait(ata_device dev, uint8_t bit, uint32_t timeout);
bool ata_wait_busy(ata_device dev, uint32_t time);
void ata_io_wait();
bool ata_select_drv(ata_device dev, uint8_t flags, uint8_t lba24_head);