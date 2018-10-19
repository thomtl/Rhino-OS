#pragma once

#include <stdint.h>
#include <rhino/arch/x86/drivers/rtc.h>
#include <rhino/arch/x86/drivers/screen.h>
#include <rhino/common.h>
#include <rhino/arch/x86/isr.h>
#include <rhino/mm/pmm.h>
#include <rhino/arch/x86/drivers/dma.h>

#define FLOPPY_IRQ IRQ6

#define FDD_TYPE_360kb_512 0
#define FDD_TYPE_12mb_512 1
#define FDD_TYPE_720kb_35 2
#define FDD_TYPE_144mb_35 3

#define FDC_SECTORS_PER_TRACK 18

typedef struct fdd_type {
  bool present;
  uint16_t storage_size; // in kilobyte
  uint16_t physical_size; // in inches
} fdd_type_t;

enum FDCDSK_IO {
  FDCDSK_DOR		=	0x3f2,
	FDCDSK_MSR		=	0x3f4,
	FDCDSK_FIFO		=	0x3f5,
	FDCDSK_CTRL		=	0x3f7
};

enum FDCDSK_DOR_MASK {
  FDCDSK_DOR_MASK_DRIVE0			=	0,	//00000000	= here for completeness sake
	FDCDSK_DOR_MASK_DRIVE1			=	1,	//00000001
	FDCDSK_DOR_MASK_DRIVE2			=	2,	//00000010
	FDCDSK_DOR_MASK_DRIVE3			=	3,	//00000011
	FDCDSK_DOR_MASK_RESET			=	4,	//00000100
	FDCDSK_DOR_MASK_DMA			=	8,	//00001000
	FDCDSK_DOR_MASK_DRIVE0_MOTOR		=	16,	//00010000
	FDCDSK_DOR_MASK_DRIVE1_MOTOR		=	32,	//00100000
	FDCDSK_DOR_MASK_DRIVE2_MOTOR		=	64,	//01000000
	FDCDSK_DOR_MASK_DRIVE3_MOTOR		=	128	//10000000
};

enum FDCDSK_MSR_MASK {
  FDCDSK_MSR_MASK_DRIVE1_POS_MODE	=	1,	//00000001
	FDCDSK_MSR_MASK_DRIVE2_POS_MODE	=	2,	//00000010
	FDCDSK_MSR_MASK_DRIVE3_POS_MODE	=	4,	//00000100
	FDCDSK_MSR_MASK_DRIVE4_POS_MODE	=	8,	//00001000
	FDCDSK_MSR_MASK_BUSY			=	16,	//00010000
	FDCDSK_MSR_MASK_DMA			=	32,	//00100000
	FDCDSK_MSR_MASK_DATAIO			=	64, 	//01000000
	FDCDSK_MSR_MASK_DATAREG		=	128	//10000000
};

enum FDCDSK_CMD {

	FDC_CMD_READ_TRACK	=	2,
	FDC_CMD_SPECIFY		=	3,
	FDC_CMD_CHECK_STAT	=	4,
	FDC_CMD_WRITE_SECT	=	5,
	FDC_CMD_READ_SECT	=	6,
	FDC_CMD_CALIBRATE	=	7,
	FDC_CMD_CHECK_INT	=	8,
	FDC_CMD_WRITE_DEL_S	=	9,
	FDC_CMD_READ_ID_S	=	0xa,
	FDC_CMD_READ_DEL_S	=	0xc,
	FDC_CMD_FORMAT_TRACK	=	0xd,
	FDC_CMD_SEEK		=	0xf
};

enum FDCDSK_CMD_EXT {

	FDC_CMD_EXT_SKIP	=	0x20,	//00100000
	FDC_CMD_EXT_DENSITY	=	0x40,	//01000000
	FDC_CMD_EXT_MULTITRACK	=	0x80	//10000000
};

enum FDCDSK_GAP3_LENGTH {

	FDCDSK_GAP3_LENGTH_STD = 42,
	FDCDSK_GAP3_LENGTH_5_14= 32,
	FDCDSK_GAP3_LENGTH_3_5= 27
};

enum FDCDSK_SECTOR_DTL {

	FDCDSK_SECTOR_DTL_128	=	0,
	FDCDSK_SECTOR_DTL_256	=	1,
	FDCDSK_SECTOR_DTL_512	=	2,
	FDCDSK_SECTOR_DTL_1024	=	4
};

void init_fdc();
void fdc_write_dor(uint8_t val);
uint8_t fdc_read_status();
void fdc_send_command(uint8_t cmd);
uint8_t fdc_read_data();

void fdc_read_sector_imp(uint8_t head, uint8_t track, uint8_t sector);

void fdc_write_ccr(uint8_t val);

void fdc_drive_data(uint32_t stepr, uint32_t loadt, uint32_t unloadt, bool dma);
int fdc_calibrate(uint32_t drive);
void fdc_check_int(uint32_t* st0, uint32_t* cyl);

int fdc_seek(uint32_t cyl, uint32_t head);

void fdc_disable_controller();
void fdc_enable_controller();

void fdc_reset();

void fdc_lba_chs(int lba, int* head, int* track, int* sector);

uint8_t* fdc_read_sector(int sectorLBA);

void fdc_control_motor(bool b);

void display_floppy_drive_info();
