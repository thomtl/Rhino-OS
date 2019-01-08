#include <rhino/debug.h>

void debug_log_int(char* m){
    serial_write_string(COM1, m);
}