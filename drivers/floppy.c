#include "floppy.h"

uint8_t count_floppy_drives(){
    return read_cmos(0x10);
}