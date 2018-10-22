#include <rhino/arch/x86/drivers/pic.h>

uint16_t __pic_read_irq_reg(uint8_t ocw3);
uint16_t pic_get_irr();
uint16_t pic_get_isr();

uint16_t __pic_read_irq_reg(uint8_t ocw3){
    outb(PIC1_CMD, ocw3);
    outb(PIC2_CMD, ocw3);
    return (inb(PIC2_CMD) << 8) | inb(PIC1_CMD);
}

uint16_t pic_get_irr(){
    return __pic_read_irq_reg(PIC_READ_IRR);
}

uint16_t pic_get_isr(){
    return __pic_read_irq_reg(PIC_READ_ISR);
}

void pic_remap(){
    outb(PIC1_CMD, PIC_INIT); // Send Init To PIC1 and PIC2
    outb(PIC2_CMD, PIC_INIT);
    outb(PIC1_DAT, 32); // Send vector offset 32 to PIC1 and 40 to PIC2
    outb(PIC2_DAT, 40);
    outb(PIC1_DAT, 0x04); // Setup Cascade mode for PIC2 through PIC1
    outb(PIC2_DAT, 0x02);
    outb(PIC1_DAT, PIC_8086); // Tell PIC1 and PIC2 that its is operating in 8086 mode
    outb(PIC2_DAT, PIC_8086);
    outb(PIC1_DAT, 0x0); // Unmask all interrupts
    outb(PIC2_DAT, 0x0);
}

void pic_send_eoi(){
    uint16_t isr = pic_get_isr();
    // Check for PIC1 Spurious
    if(isr == 0) return;
    // Check for PIC2 Spurious
    if(BIT_IS_SET(isr, 2) && ((isr >> 8) == 0)){
        outb(PIC1_CMD, PIC_EOI);
        return;
    }
    // Send normal EOI
    if(BIT_IS_SET(isr, 2)) outb(PIC2_CMD, PIC_EOI);
    outb(PIC1_CMD, PIC_EOI);
}

void pic_disable(){
    // First disable PIC2 so PIC1 can't receive an interrupt while it is disabled
    outb(PIC2_DAT, PIC_DISABLE); // Disable PIC2
    outb(PIC1_DAT, PIC_DISABLE); // Disable PIC1
}