#pragma once

#include <rhino/common.h>
#include <rhino/arch/x86/io.h>

#define PIC1_CMD 0x20
#define PIC1_DAT 0x21
#define PIC2_CMD 0xA0
#define PIC2_DAT 0xA1
#define PIC_READ_IRR 0xA
#define PIC_READ_ISR 0xB
#define PIC_EOI 0x20
#define PIC_DISABLE 0xFF
#define PIC_INIT 0x11
#define PIC_8086 0x01

void pic_remap(uint8_t pic1_vector_offset, uint8_t pic2_vector_offset);
void pic_send_eoi();
void pic_disable();