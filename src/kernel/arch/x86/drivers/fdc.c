#include <rhino/arch/x86/drivers/fdc.h>

volatile bool fdc_irq_fired = 0;

fdd_type_t master, slave;

uint32_t* dma_frame;

uint32_t _CurrentDrive = 0;

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
  //memset((void*)((uint32_t)dma_frame + (uint32_t)KERNEL_VBASE), 0xABCDEFF, 0x1000);
  fdc_reset();

  fdc_drive_data(13, 1, 0xf, true);

}

void fdc_write_dor(uint8_t val){
  outb(FDCDSK_DOR, val);
}

uint8_t fdc_read_status(){
  return inb(FDCDSK_MSR);
}

void fdc_send_command(uint8_t cmd){
    for(uint16_t i = 0; i < 500; i++){
      if(fdc_read_status() & FDCDSK_MSR_MASK_DATAREG){
        outb(FDCDSK_FIFO, cmd);
        return;
      }
    }
}

uint8_t fdc_read_data(){
  for(uint16_t i = 0; i < 500; i++){
    if(fdc_read_status() & FDCDSK_MSR_MASK_DATAREG){
      return inb(FDCDSK_FIFO);
    }
  }
  return 0;
}

void fdc_write_ccr(uint8_t val){
  outb(FDCDSK_CTRL, val);
}


void fdc_read_sector_imp(uint8_t head, uint8_t track, uint8_t sector){
  uint32_t st0, cyl;

  dma_initialize_floppy((uint8_t*)dma_frame, 512);
  dma_set_read(2);
  fdc_send_command(FDC_CMD_READ_SECT | FDC_CMD_EXT_MULTITRACK | FDC_CMD_EXT_SKIP | FDC_CMD_EXT_DENSITY);
  fdc_send_command(head << 2 | _CurrentDrive);
  fdc_send_command(track);
  fdc_send_command(head);
  fdc_send_command(sector);
  fdc_send_command(FDCDSK_SECTOR_DTL_512);
  fdc_send_command (( ( sector + 1 ) >= FDC_SECTORS_PER_TRACK ) ? FDC_SECTORS_PER_TRACK : sector + 1 );
  fdc_send_command(FDCDSK_GAP3_LENGTH_3_5);
  fdc_send_command(0xFF);

  fdc_wait_irq();

  for(uint8_t i = 0; i < 7; i++){
    fdc_read_data();
  }

  fdc_check_int(&st0, &cyl);
}


void fdc_drive_data(uint32_t stepr, uint32_t loadt, uint32_t unloadt, bool dma){
  uint32_t data = 0;

  fdc_send_command(FDC_CMD_SPECIFY);

  data = ((stepr & 0xF) << 4) | (unloadt & 0xF);
  fdc_send_command(data);

  data = (loadt) << 1 | (dma) ? 0 : 1;
  fdc_send_command(data);
}

int fdc_calibrate(uint32_t drive){
  uint32_t st0, cyl;

  if(drive >= 4) return -2;

  fdc_control_motor(true);

  for(int i = 0; i < 10; i++){
    fdc_send_command(FDC_CMD_CALIBRATE);
    fdc_send_command(drive);
    fdc_wait_irq();
    fdc_check_int(&st0, &cyl);

    if(!cyl){
      fdc_control_motor(false);
      return 0;
    }
  }
  fdc_control_motor(false);
  return -1;
}

void fdc_check_int(uint32_t* st0, uint32_t* cyl){
  fdc_send_command(FDC_CMD_CHECK_INT);

  *st0 = fdc_read_data();
  *cyl = fdc_read_data();
}

int fdc_seek(uint32_t cyl, uint32_t head){
  uint32_t st0, cyl0;

  if(_CurrentDrive >= 4) return -1;

  for(int i = 0; i < 10; i++){
    fdc_send_command(FDC_CMD_SEEK);
    fdc_send_command( (head) << 2 | _CurrentDrive);
    fdc_send_command(cyl);
    fdc_wait_irq();
    fdc_check_int(&st0, &cyl0);
    if(cyl0 == cyl) return 0;
  }

  return -1;
}

void fdc_disable_controller(){
  fdc_write_dor(0);
}

void fdc_enable_controller(){
  fdc_write_dor(FDCDSK_DOR_MASK_RESET | FDCDSK_DOR_MASK_DMA);
}

void fdc_reset(){
  uint32_t st0, cyl;

  fdc_disable_controller();
  fdc_enable_controller();

  fdc_wait_irq();

  for(int i = 0; i < 4; i++) fdc_check_int(&st0, &cyl);

  fdc_write_ccr(0);

  fdc_drive_data(3, 16, 240, true);

  fdc_calibrate(_CurrentDrive);
}

void fdc_lba_chs(int lba, int* head, int* track, int* sector){
    *head = (lba % (FDC_SECTORS_PER_TRACK * 2)) / FDC_SECTORS_PER_TRACK;
    *track = lba / (FDC_SECTORS_PER_TRACK * 2);
    *sector = lba % FDC_SECTORS_PER_TRACK + 1;
}

uint8_t* fdc_read_sector(int sectorLBA){
  if(_CurrentDrive >= 4) return 0;

  int head, track, sector;

  fdc_lba_chs(sectorLBA, &head, &track, &sector);
  fdc_control_motor(true);
  if(fdc_seek(track, head) != 0) return 0;

  fdc_read_sector_imp(head, track, sector);

  fdc_control_motor(false);

  return (uint8_t*)dma_frame;
}

void fdc_control_motor(bool b){
  if(_CurrentDrive > 3) return;

  uint32_t motor = 0;

  switch (_CurrentDrive) {
    case 0:
      motor = FDCDSK_DOR_MASK_DRIVE0_MOTOR;
      break;
    case 1:
      motor = FDCDSK_DOR_MASK_DRIVE1_MOTOR;
      break;
    case 2:
      motor = FDCDSK_DOR_MASK_DRIVE2_MOTOR;
      break;
    case 3:
      motor = FDCDSK_DOR_MASK_DRIVE3_MOTOR;
      break;
    default:
      return;
  }

  if(b) fdc_write_dor(_CurrentDrive | motor | FDCDSK_DOR_MASK_RESET | FDCDSK_DOR_MASK_DMA);
  else fdc_write_dor(FDCDSK_DOR_MASK_RESET);

  for(volatile int i = 0; i < 999; i++); // Delay because no sleep yet
}
