#include <rhino/arch/x86/drivers/ioapic.h>
uint32_t ioapic_base;
static void ioapic_write(uint8_t reg, uint32_t val)
{
    mmio_write32((uint32_t*)(ioapic_base + IOREGSEL), reg);
    mmio_write32((uint32_t*)(ioapic_base + IOWIN), val);
}

static uint32_t ioapic_read(uint8_t reg)
{
    mmio_write32((uint32_t*)(ioapic_base + IOREGSEL), reg);
    return mmio_read32((uint32_t*)(ioapic_base + IOWIN));
}

void init_ioapic(uint32_t base){
    ioapic_base = base;
    uint32_t ver = ioapic_read(IOAPICVER);
    uint32_t maxCount = ((ver >> 16) & 0xff) + 1;

    for(uint32_t i = 0; i < maxCount; i++){
        ioapic_set_entry(0, 1 << 16);
    }
    // All entries masked
}

void ioapic_set_entry(uint8_t index, uint64_t data){
    ioapic_write(IOREDTBL + index * 2, (uint32_t)(data & 0xFFFFFFFF));
    ioapic_write(IOREDTBL + index * 2 + 1, (uint32_t)(data >> 32));
}