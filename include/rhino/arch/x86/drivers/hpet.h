#pragma once
#include <rhino/common.h>
#include <rhino/arch/x86/ports.h>
#include <rhino/acpi/acpi.h>
#include <rhino/acpi/acpistructs.h>
#include <rhino/mm/vmm.h>

#include <rhino/arch/x86/drivers/screen.h>

#define HPET_GENERAL_CAPABILITIES_REG 0x0
#define HPET_GENERAL_CONFIGURATION_REG 0x10
#define HPET_GENERAL_INTERRUPT_STATUS_REG 0x20
#define HPET_MAIN_COUNTER_VAL 0xF0
#define HPET_TIMER_CONFIGURATION_AND_CAPABILITY_REG(n) (0x100 + 0x20 * (n))
#define HPET_TIMER_COMPARATOR_VALUE_REG(n) (0x108 + 0x20 * (n))
#define HPET_TIMER_FSB_INTERRUPT_ROUTE_REG(n) (0x110 + 0x20 * (n))

#define HPET_ACPI_SIGNATURE "HPET"

bool detect_hpet();
bool init_hpet(uint64_t frq);