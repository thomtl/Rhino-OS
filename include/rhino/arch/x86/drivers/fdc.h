#pragma once

#include <stdint.h>
#include <rhino/arch/x86/cmos.h>
#include <rhino/arch/x86/drivers/screen.h>
#include <rhino/common.h>
#include <rhino/arch/x86/isr.h>
#include <rhino/mm/pmm.h>

#define FLOPPY_IRQ IRQ6

#define FDD_TYPE_360kb_512 0
#define FDD_TYPE_12mb_512 1
#define FDD_TYPE_720kb_35 2
#define FDD_TYPE_144mb_35 3


typedef struct fdd_type {
  bool present;
  uint16_t storage_size; // in kilobyte
  uint16_t physical_size; // in inches * 100
} fdd_type_t;

void init_fdc();
void display_floppy_drive_info();
