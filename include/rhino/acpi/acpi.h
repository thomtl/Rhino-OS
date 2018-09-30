#pragma once
#include <rhino/common.h>
#include <rhino/acpi/acpistructs.h>
#include <rhino/mm/vmm.h>
#include <rhino/arch/x86/cpuid.h>
#include <rhino/arch/x86/drivers/screen.h>
#include <rhino/arch/x86/bios.h>

#define ACPI_RSDT_SIG "RSD PTR "
void init_acpi();
uint32_t acpi_get_fadt_boot_arch_flags();
uint32_t acpi_get_fadt_version();
bool acpi_table_exists(char* signature);
void* find_table(char* signature);