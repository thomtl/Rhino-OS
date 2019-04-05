#include <rhino/debug.h>

#ifdef DEBUG

void debug_log_int(char *m)
{
    serial_write_string(COM1, m);
}

void debug_log_number_dec_int(uint32_t num)
{
    char buf[25] = "";
    int_to_ascii(num, buf);
    serial_write_string(COM1, buf);
}

void debug_log_number_hex_int(uint32_t num)
{
    char buf[25] = "";
    hex_to_ascii(num, buf);
    serial_write_string(COM1, buf);
}

#endif