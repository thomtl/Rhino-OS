#pragma once
#include <stdint.h>
#include <rhino/acpi/acpi.h>
#include <rhino/arch/x86/cpuid.h>

void reboot();
void shutdown();
extern void reboot_8042();