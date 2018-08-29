#include <rhino/arch/x86/drivers/fdc.h>

volatile bool fdc_irq_fired = 0;

fdd_type_t master, slave;

uint32_t* dma_frame;

void display_floppy_drive_info(){
  uint8_t flop = read_cmos(CMOS_REG_INT_FLOPPY_DATA);

  if(BIT_IS_SET(flop, 4) && !BIT_IS_SET(flop, 5) && !BIT_IS_SET(flop, 6) && !BIT_IS_SET(flop, 7)){
    kprint_warn("Master 360 KB 5.25 Drive\n");
  } else if(!BIT_IS_SET(flop, 4) && BIT_IS_SET(flop, 5) && !BIT_IS_SET(flop, 6) && !BIT_IS_SET(flop, 7)){
    kprint_warn("Master 1.2 MB 5.25 Drive\n");
  } else if(BIT_IS_SET(flop, 4) && BIT_IS_SET(flop, 5) && !BIT_IS_SET(flop, 6) && !BIT_IS_SET(flop, 7)){
    kprint_warn("Master 720 KB 3.5 Drive\n");
  } else if(!BIT_IS_SET(flop, 4) && !BIT_IS_SET(flop, 5) && BIT_IS_SET(flop, 6) && !BIT_IS_SET(flop, 7)){
    kprint_warn("Master 1.44 MB 3.5 Drive\n");
  } else if(BIT_IS_SET(flop, 4) && !BIT_IS_SET(flop, 5) && BIT_IS_SET(flop, 6) && !BIT_IS_SET(flop, 7)){
    kprint_warn("Master 1.44 MB 3.5 Drive\n");
  } else {
    kprint_warn("No Master floppy drive detected\n");
  }

  if(BIT_IS_SET(flop, 0) && !BIT_IS_SET(flop, 1) && !BIT_IS_SET(flop, 2) && !BIT_IS_SET(flop, 3)){
    kprint_warn("Slave 360 KB 5.25 Drive\n");
  } else if(!BIT_IS_SET(flop, 0) && BIT_IS_SET(flop, 1) && !BIT_IS_SET(flop, 2) && !BIT_IS_SET(flop, 3)){
    kprint_warn("Slave 1.2 MB 5.25 Drive\n");
  } else if(BIT_IS_SET(flop, 0) && BIT_IS_SET(flop, 1) && !BIT_IS_SET(flop, 2) && !BIT_IS_SET(flop, 3)){
    kprint_warn("Slave 720 KB 3.5 Drive\n");
  } else if(!BIT_IS_SET(flop, 0) && !BIT_IS_SET(flop, 1) && BIT_IS_SET(flop, 2) && !BIT_IS_SET(flop, 3)){
    kprint_warn("Slave 1.44 MB 3.5 Drive\n");
  } else if(BIT_IS_SET(flop, 0) && !BIT_IS_SET(flop, 1) && BIT_IS_SET(flop, 2) && !BIT_IS_SET(flop, 3)){
    kprint_warn("Slave 1.44 MB 3.5 Drive\n");
  } else {
    kprint_warn("No Slave floppy drive detected\n");
  }


}

void fdc_irq(registers_t* r){
  fdc_irq_fired = 1;
  (void)(r);
}

void fdc_wait_irq(){
    while(fdc_irq_fired == 0);
    fdc_irq_fired = 0;
}

void init_fdc(){
  register_interrupt_handler(FLOPPY_IRQ, fdc_irq);


  uint8_t fdc_fdd_byte = read_cmos(CMOS_REG_INT_FLOPPY_DATA);

  if(BIT_IS_SET(fdc_fdd_byte, 4) && !BIT_IS_SET(fdc_fdd_byte, 5) && !BIT_IS_SET(fdc_fdd_byte, 6) && !BIT_IS_SET(fdc_fdd_byte, 7)){
    master.present = true;
    master.physical_size = 5.25;
    master.storage_size = 360;
  } else if(!BIT_IS_SET(fdc_fdd_byte, 4) && BIT_IS_SET(fdc_fdd_byte, 5) && !BIT_IS_SET(fdc_fdd_byte, 6) && !BIT_IS_SET(fdc_fdd_byte, 7)){
    master.present = true;
    master.physical_size = 5.25;
    master.storage_size = 1200;
  } else if(BIT_IS_SET(fdc_fdd_byte, 4) && BIT_IS_SET(fdc_fdd_byte, 5) && !BIT_IS_SET(fdc_fdd_byte, 6) && !BIT_IS_SET(fdc_fdd_byte, 7)){
    master.present = true;
    master.physical_size = 3.5;
    master.storage_size = 720;
  } else if(BIT_IS_SET(fdc_fdd_byte, 4) && !BIT_IS_SET(fdc_fdd_byte, 5) && BIT_IS_SET(fdc_fdd_byte, 6) && !BIT_IS_SET(fdc_fdd_byte, 7)){
    master.present = true;
    master.physical_size = 3.5;
    master.storage_size = 1440;
  } else {
    master.present = false;
  }

  if(BIT_IS_SET(fdc_fdd_byte, 0) && !BIT_IS_SET(fdc_fdd_byte, 1) && !BIT_IS_SET(fdc_fdd_byte, 2) && !BIT_IS_SET(fdc_fdd_byte, 3)){
    slave.present = true;
    slave.physical_size = 5.25;
    slave.storage_size = 360;
  } else if(!BIT_IS_SET(fdc_fdd_byte, 0) && BIT_IS_SET(fdc_fdd_byte, 1) && !BIT_IS_SET(fdc_fdd_byte, 2) && !BIT_IS_SET(fdc_fdd_byte, 3)){
    slave.present = true;
    slave.physical_size = 5.25;
    slave.storage_size = 1200;
  } else if(BIT_IS_SET(fdc_fdd_byte, 0) && BIT_IS_SET(fdc_fdd_byte, 1) && !BIT_IS_SET(fdc_fdd_byte, 2) && !BIT_IS_SET(fdc_fdd_byte, 3)){
    slave.present = true;
    slave.physical_size = 3.5;
    slave.storage_size = 720;
  } else if(BIT_IS_SET(fdc_fdd_byte, 0) && !BIT_IS_SET(fdc_fdd_byte, 1) && BIT_IS_SET(fdc_fdd_byte, 2) && !BIT_IS_SET(fdc_fdd_byte, 3)){
    slave.present = true;
    slave.physical_size = 3.5;
    slave.storage_size = 1440;
  } else {
    slave.present = false;
  }

  dma_frame = (uint32_t*)pmm_alloc_block();
  pmm_alloc_block();
  pmm_alloc_block();


}
