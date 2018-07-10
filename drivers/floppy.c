#include "floppy.h"

void display_floppy_drive_info(){
  uint8_t flop = read_cmos(CMOS_REG_FLOPPY_DATA);
  
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
