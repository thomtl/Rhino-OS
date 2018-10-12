#include <rhino/arch/x86/drivers/fpu.h>

void fpu_exception(registers_t* regs){
    uint16_t status = fpu_get_status_word();
    kprint("AF");
    if(BIT_IS_SET(status, 7)){
        if(BIT_IS_SET(status, 0)) {
            kprint_err("[FPU]: Invalid Operation");
            while(1);
        } else if(BIT_IS_SET(status, 1)) {
            kprint_err("[FPU]: Denormalized Exception");
            while(1);
        } else if(BIT_IS_SET(status, 2)) {
            kprint_err("[FPU]: Divide by zero error");
            while(1);
        } else if(BIT_IS_SET(status, 3)) {
            kprint_err("[FPU]: Overflow Exception");
            while(1);
        } else if(BIT_IS_SET(status, 4)) {
            kprint_err("[FPU]: Underflow Exception");
            while(1);
        } else if(BIT_IS_SET(status, 5)) {
            kprint_err("[FPU]: Precision Exception");
            while(1);
        } else if(BIT_IS_SET(status, 6)) {
            kprint_err("[FPU]: Stack Fault");
            while(1);
        }
    } else {
        kprint_warn("[FPU]: Got interrupt 0x10(FPU Exception) but bit 7 of status register is not set\n");
    }
    UNUSED(regs);
}

bool init_fpu(){
    uint32_t eax, ecx, edx;
    cpuid(1, &eax, &ecx, &edx);
    if(!BIT_IS_SET(edx, CPUID_FEAT_EDX_FPU)){
        return false;
    }

    if(!BIT_IS_SET(edx, CPUID_FEAT_EDX_FXSR)){
        return false;
    }

    if(probe_fpu() == 0){
        return false;
    }
    register_interrupt_handler(0x10, fpu_exception);
    init_fpu_low();

    return true;
}

uint8_t saves[512] __attribute__((aligned(16)));

void save_fpu_context(task_t* task){
    asm volatile ("fxsave (%0)" :: "r"(saves));
	memcpy((uint8_t *)task->regs.fpu_regs,&saves,512);
}

void restore_fpu_context(task_t* task){
    memcpy(&saves,(uint8_t *)task->regs.fpu_regs,512);
	asm volatile ("fxrstor (%0)" :: "r"(saves));
}