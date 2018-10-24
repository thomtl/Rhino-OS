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

bool detect_apic();
bool init_apic();
uint32_t apic_irq_redirect(uint8_t IRQ);