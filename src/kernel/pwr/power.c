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

void shutdown(){
    if(detectACPI()) acpi_shutdown();
    else {
        if(is_hypervisor()){
            // Running on a Hypervisor or Emulator so try some weird things
            for (const char *s = "Shutdown"; *s; ++s) { // Old QEMU / Bochs Things
                outb(0x8900, *s);
            }
        }

    }
}