#include <rhino/arch/x86/isr.h>
#include <rhino/arch/x86/idt.h>
#include <rhino/arch/x86/drivers/screen.h>
#include <rhino/arch/x86/drivers/keyboard.h>
#include <libk/string.h>
#include <rhino/arch/x86/timer.h>
#include <rhino/arch/x86/ports.h>
#include <rhino/user/syscall.h>
isr_t interrupt_handlers[256];

void isr_install() {
    set_idt_gate(0, (uint32_t)isr0, KERNEL_CS, 0);
    set_idt_gate(1, (uint32_t)isr1, KERNEL_CS, 0);
    set_idt_gate(2, (uint32_t)isr2, KERNEL_CS, 0);
    set_idt_gate(3, (uint32_t)isr3, KERNEL_CS, 0);
    set_idt_gate(4, (uint32_t)isr4, KERNEL_CS, 0);
    set_idt_gate(5, (uint32_t)isr5, KERNEL_CS, 0);
    set_idt_gate(6, (uint32_t)isr6, KERNEL_CS, 0);
    set_idt_gate(7, (uint32_t)isr7, KERNEL_CS, 0);
    set_idt_gate(8, (uint32_t)isr8, KERNEL_CS, 0);
    set_idt_gate(9, (uint32_t)isr9, KERNEL_CS, 0);
    set_idt_gate(10, (uint32_t)isr10, KERNEL_CS, 0);
    set_idt_gate(11, (uint32_t)isr11, KERNEL_CS, 0);
    set_idt_gate(12, (uint32_t)isr12, KERNEL_CS, 0);
    set_idt_gate(13, (uint32_t)isr13, KERNEL_CS, 0);
    set_idt_gate(14, (uint32_t)isr14, KERNEL_CS, 0);
    set_idt_gate(15, (uint32_t)isr15, KERNEL_CS, 0);
    set_idt_gate(16, (uint32_t)isr16, KERNEL_CS, 0);
    set_idt_gate(17, (uint32_t)isr17, KERNEL_CS, 0);
    set_idt_gate(18, (uint32_t)isr18, KERNEL_CS, 0);
    set_idt_gate(19, (uint32_t)isr19, KERNEL_CS, 0);
    set_idt_gate(20, (uint32_t)isr20, KERNEL_CS, 0);
    set_idt_gate(21, (uint32_t)isr21, KERNEL_CS, 0);
    set_idt_gate(22, (uint32_t)isr22, KERNEL_CS, 0);
    set_idt_gate(23, (uint32_t)isr23, KERNEL_CS, 0);
    set_idt_gate(24, (uint32_t)isr24, KERNEL_CS, 0);
    set_idt_gate(25, (uint32_t)isr25, KERNEL_CS, 0);
    set_idt_gate(26, (uint32_t)isr26, KERNEL_CS, 0);
    set_idt_gate(27, (uint32_t)isr27, KERNEL_CS, 0);
    set_idt_gate(28, (uint32_t)isr28, KERNEL_CS, 0);
    set_idt_gate(29, (uint32_t)isr29, KERNEL_CS, 0);
    set_idt_gate(30, (uint32_t)isr30, KERNEL_CS, 0);
    set_idt_gate(31, (uint32_t)isr31, KERNEL_CS, 0);

    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0x0);
    outb(0xA1, 0x0);

    set_idt_gate(32, (uint32_t)irq0, KERNEL_CS, 0);
    set_idt_gate(33, (uint32_t)irq1, KERNEL_CS, 0);
    set_idt_gate(34, (uint32_t)irq2, KERNEL_CS, 0);
    set_idt_gate(35, (uint32_t)irq3, KERNEL_CS, 0);
    set_idt_gate(36, (uint32_t)irq4, KERNEL_CS, 0);
    set_idt_gate(37, (uint32_t)irq5, KERNEL_CS, 0);
    set_idt_gate(38, (uint32_t)irq6, KERNEL_CS, 0);
    set_idt_gate(39, (uint32_t)irq7, KERNEL_CS, 0);
    set_idt_gate(40, (uint32_t)irq8, KERNEL_CS, 0);
    set_idt_gate(41, (uint32_t)irq9, KERNEL_CS, 0);
    set_idt_gate(42, (uint32_t)irq10, KERNEL_CS, 0);
    set_idt_gate(43, (uint32_t)irq11, KERNEL_CS, 0);
    set_idt_gate(44, (uint32_t)irq12, KERNEL_CS, 0);
    set_idt_gate(45, (uint32_t)irq13, KERNEL_CS, 0);
    set_idt_gate(46, (uint32_t)irq14, KERNEL_CS, 0);
    set_idt_gate(47, (uint32_t)irq15, KERNEL_CS, 0);
    set_idt_gate(128, (uint32_t)isr128, KERNEL_CS, 3);
    set_idt(); // Load with ASM
}

char *exception_messages[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",

    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",

    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",

    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

void isr_handler(registers_t *r){
  uint8_t int_no = r->int_no & 0xFF;
  if(interrupt_handlers[int_no] != 0){
    isr_t handler = interrupt_handlers[int_no];
    handler(r);
  } else {
    kprint_err("received interrupt: ");
    char s[3];
    int_to_ascii(int_no, s);
    kprint_err(s);
    kprint_err("\n");
    kprint_err(exception_messages[int_no]);
    kprint_err("\n");
    while(1);
  }
}

void register_interrupt_handler(uint8_t n, isr_t handler){
  interrupt_handlers[n] = handler;
}

void irq_handler(registers_t *r){

  if(interrupt_handlers[r->int_no] != 0){
    isr_t handler = interrupt_handlers[r->int_no];
    handler(r);
  }

  if(r->int_no >= 40) outb(0xA0, 0x20);
  outb(0x20, 0x20);

}

void irq_install(){
  init_timer();
  init_keyboard();
  init_syscall();
  asm("sti");
}

void enable_nmi(){
  outb(0x70, inb(0x70) & 0x7F);
}

void disable_nmi(){
  outb(0x70, inb(0x70) | 0x80);
}