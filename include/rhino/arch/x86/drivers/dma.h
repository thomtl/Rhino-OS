#pragma once

#include <rhino/arch/x86/ports.h>

void fdc_setup_transfer(uint32_t frame);
void fdc_read();
void fdc_write();
