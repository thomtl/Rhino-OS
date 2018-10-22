// x86 APIC Driver
#pragma once

#include <rhino/common.h>
#include <rhino/mm/vmm.h>
#include <rhino/acpi/acpi.h>
#include <rhino/arch/x86/cpuid.h>
#include <rhino/arch/x86/msr.h>
#include <rhino/arch/x86/drivers/pic.h>
#include <rhino/arch/x86/drivers/lapic.h>
#include <rhino/arch/x86/drivers/ioapic.h>
#define MADT_SIGNATURE "APIC"

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
#define LAPIC_ISR0_REG 0x100
#define LAPIC_ISR1_REG 0x110
#define LAPIC_ISR2_REG 0x120
#define LAPIC_ISR3_REG 0x130
#define LAPIC_ISR4_REG 0x140
#define LAPIC_ISR5_REG 0x150
#define LAPIC_ISR6_REG 0x160
#define LAPIC_ISR7_REG 0x170
#define LAPIC_TMR0_REG 0x180
#define LAPIC_TMR1_REG 0x190
#define LAPIC_TMR2_REG 0x1A0
#define LAPIC_TMR3_REG 0x1B0
#define LAPIC_TMR4_REG 0x1C0
#define LAPIC_TMR5_REG 0x1D0
#define LAPIC_TMR6_REG 0x1E0
#define LAPIC_TMR7_REG 0x1F0
#define LAPIC_IRR0_REG 0x200
#define LAPIC_IRR1_REG 0x210
#define LAPIC_IRR2_REG 0x220
#define LAPIC_IRR3_REG 0x230
#define LAPIC_IRR4_REG 0x240
#define LAPIC_IRR5_REG 0x250
#define LAPIC_IRR6_REG 0x260
#define LAPIC_IRR7_REG 0x270
#define LAPIC_ERROR_STATUS_REG 0x280
#define LAPIC_LVT_CMCI_REG 0x2F0
#define LAPIC_ICR0_REG 0x300
#define LAPIC_ICR1_REG 0x310

bool detect_apic();
bool init_apic();
void apic_send_eoi();
uint32_t apic_redirect(uint8_t IRQ);