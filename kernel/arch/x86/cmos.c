#include "cmos.h"

uint8_t read_cmos(uint8_t reg){
    CLI();
    outb(CMOS_REG_SELECT, (CMOS_NMI_DISABLE_BIT << 7) | reg);
    uint8_t ret = inb(CMOS_REG_DATA);
    STI();
    return ret;
}

void write_cmos(uint8_t reg, uint8_t val){
    CLI();
    outb(CMOS_REG_SELECT, (CMOS_NMI_DISABLE_BIT << 7) | reg);
    outb(CMOS_REG_DATA, val);
    STI();
}

int get_update_in_progress_flag() {
      outb(CMOS_REG_SELECT, CMOS_REG_INT_STATUS_A);
      return (inb(CMOS_REG_DATA) & CMOS_NMI_DISABLE_BIT);
}

time_t read_rtc() {
      time_t ret;
      unsigned char last_second;
      unsigned char last_minute;
      unsigned char last_hour;
      unsigned char last_day;
      unsigned char last_month;
      unsigned char last_year;
      unsigned char registerB;

      // Note: This uses the "read registers until you get the same values twice in a row" technique
      //       to avoid getting dodgy/inconsistent values due to RTC updates

      while (get_update_in_progress_flag());                // Make sure an update isn't in progress
      ret.second = read_cmos(CMOS_REG_INT_SECONDS);
      ret.minute = read_cmos(CMOS_REG_INT_MINUTES);
      ret.hour = read_cmos(CMOS_REG_INT_HOURS);
      ret.day = read_cmos(CMOS_REG_INT_DAYOFMONTH);
      ret.month = read_cmos(CMOS_REG_INT_MONTH);
      ret.year = read_cmos(CMOS_REG_INT_YEAR);
      do {
            last_second = ret.second;
            last_minute = ret.minute;
            last_hour = ret.hour;
            last_day = ret.day;
            last_month = ret.month;
            last_year = ret.year;

            while (get_update_in_progress_flag());           // Make sure an update isn't in progress
            ret.second = read_cmos(CMOS_REG_INT_SECONDS);
            ret.minute = read_cmos(CMOS_REG_INT_MINUTES);
            ret.hour = read_cmos(CMOS_REG_INT_HOURS);
            ret.day = read_cmos(CMOS_REG_INT_DAYOFMONTH);
            ret.month = read_cmos(CMOS_REG_INT_MONTH);
            ret.year = read_cmos(CMOS_REG_INT_YEAR);
      } while( (last_second != ret.second) || (last_minute != ret.minute) || (last_hour != ret.hour) ||
               (last_day != ret.day) || (last_month != ret.month) || (last_year != ret.year));

      registerB = read_cmos(CMOS_REG_INT_STATUS_B);

      // Convert BCD to binary values if necessary

      if (!(registerB & 0x04)) {
            ret.second = (ret.second & 0x0F) + ((ret.second / 16) * 10);
            ret.minute = (ret.minute & 0x0F) + ((ret.minute / 16) * 10);
            //ret.hour = ( (ret.hour & 0x0F) + (((ret.hour & 0x70) / 16) * 10) ) | (ret.hour & 0x80);
            ret.hour = (ret.hour & 0x0F) + ((ret.hour / 16) * 10);
            ret.day = (ret.day & 0x0F) + ((ret.day / 16) * 10);
            ret.month = (ret.month & 0x0F) + ((ret.month / 16) * 10);
            ret.year = (ret.year & 0x0F) + ((ret.year / 16) * 10);
      }

      // Convert 12 ret.hour clock to 24 ret.hour clock if necessary

      if (!(registerB & 0x02) && (ret.hour & 0x80)) {
            ret.hour = ((ret.hour & 0x7F) + 12) % 24;
      }

      // Calculate the full (4-digit) ret.year
      ret.year += (CURRENT_YEAR / 100) * 100;
      if(ret.year < CURRENT_YEAR) ret.year += 100;
      return ret;
}
