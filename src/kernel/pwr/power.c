#include <rhino/pwr/power.h>

void reboot(){
    if(detectACPI()){
        bool a = acpi_reboot();
        if(!a) goto legacy;
    } else {
        legacy:
        reboot_8042();
    }
}