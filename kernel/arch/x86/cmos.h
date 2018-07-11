#ifndef CMOS_h
#define CMOS_h

#include <stdint.h>
#include "ports.h"
#include "../../common.h"
#define CMOS_NMI_DISABLE_BIT 0x80
#define CMOS_REG_SELECT 0x70
#define CMOS_REG_DATA 0x71
#define CURRENT_YEAR 2018
#define CMOS_REG_INT_SECONDS 0x00
#define CMOS_REG_INT_MINUTES 0x02
#define CMOS_REG_INT_HOURS 0x04
#define CMOS_REG_INT_WEEKDAY 0x06
#define CMOS_REG_INT_DAYOFMONTH 0x07
#define CMOS_REG_INT_MONTH 0x08
#define CMOS_REG_INT_YEAR 0x09
#define CMOS_REG_INT_CENTURY 0x32
#define CMOS_REG_INT_STATUS_A 0x0A
#define CMOS_REG_INT_STATUS_B 0x0B
#define CMOS_REG_INT_FLOPPY_DATA 0x0B

typedef struct {
  uint8_t second;
  uint8_t minute;
  uint8_t hour;
  uint8_t day;
  uint8_t month;
  uint32_t year;
} time_t;


uint8_t read_cmos(uint8_t reg);
void write_cmos(uint8_t reg, uint8_t val);
time_t read_rtc();


#endif
