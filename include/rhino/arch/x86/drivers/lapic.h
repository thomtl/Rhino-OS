// x86 LAPIC driver
#pragma once
#include <rhino/common.h>
#include <rhino/mm/mmio.h>


#define LAPIC_ID_REG 0x20
#define LAPIC_VERSION_REG 0x30
#define LAPIC_TPR_REG 0x80
#define LAPIC_APR_REG 0x90
#define LAPIC_PPR_REG 0xA0
#define LAPIC_EOI_REG 0xB0
#define LAPIC_RRD_REG 0xC0
#define LAPIC_LDR_REG 0xD0
#define LAPIC_DFR_REG 0xE0
#define LAPIC_SPURIOUS_VECTOR_REG 0xF0
#define LAPIC_ISR_REG 0x100 // 8 regs total
#define LAPIC_TMR_REG 0x180 // 8 regs total
#define LAPIC_IRR_REG 0x200 // 8 regs total
#define LAPIC_ERROR_STATUS_REG 0x280
#define LAPIC_LVT_CMCI_REG 0x2F0
#define LAPIC_ICR0_REG 0x300
#define LAPIC_ICR1_REG 0x310
#define LAPIC_TIMER                     0x0320
#define LAPIC_THERMAL                   0x0330
#define LAPIC_PERF                      0x0340
#define LAPIC_LINT0                     0x0350
#define LAPIC_LINT1                     0x0360
#define LAPIC_ERROR                     0x0370
#define LAPIC_TICR                      0x0380
#define LAPIC_TCCR                      0x0390
#define LAPIC_TDCR                      0x03e0

#define ICR_FIXED                       0x00000000
#define ICR_LOWEST                      0x00000100
#define ICR_SMI                         0x00000200
#define ICR_NMI                         0x00000400
#define ICR_INIT                        0x00000500
#define ICR_STARTUP                     0x00000600

#define ICR_PHYSICAL                    0x00000000
#define ICR_LOGICAL                     0x00000800

#define ICR_IDLE                        0x00000000
#define ICR_SEND_PENDING                0x00001000

#define ICR_DEASSERT                    0x00000000
#define ICR_ASSERT                      0x00004000

#define ICR_EDGE                        0x00000000
#define ICR_LEVEL                       0x00008000

#define ICR_NO_SHORTHAND                0x00000000
#define ICR_SELF                        0x00040000
#define ICR_ALL_INCLUDING_SELF          0x00080000
#define ICR_ALL_EXCLUDING_SELF          0x000c0000

#define ICR_DESTINATION_SHIFT           24

void init_lapic(uint32_t base);
uint32_t lapic_get_id();
void lapic_send_eoi();