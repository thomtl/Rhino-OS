// Manage MMIO
#include <rhino/common.h>
#include <rhino/mm/vmm.h>

uint8_t mmio_read8(uint32_t* addr);
void mmio_write8(uint32_t* addr, uint8_t val);
uint16_t mmio_read16(uint32_t* addr);
void mmio_write16(uint32_t* addr, uint16_t val);
uint32_t mmio_read32(uint32_t* addr);
void mmio_write32(uint32_t* addr, uint32_t val);
uint64_t mmio_read64(uint32_t* addr);
void mmio_write64(uint32_t* addr, uint64_t val);