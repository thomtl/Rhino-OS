#include <rhino/arch/x86/bios.h>

void* get_ebda_base(){
    uint32_t ebda = *((short*)(0x40E  + KERNEL_VBASE));
    ebda = ebda * 0x10 & 0x000FFFFF;
    if(ebda == 0){
        return (void*)BIOS_USUAL_EBDA_ADDR;
    }
    return (void*)ebda;
}