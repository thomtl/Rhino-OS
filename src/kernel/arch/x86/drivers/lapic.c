#include <rhino/arch/x86/drivers/lapic.h>
uint32_t lapic_base;
static uint32_t lapic_read(uint32_t reg)
{
    return mmio_read32((uint32_t*)(lapic_base + reg));
}

static void lapic_write(uint32_t reg, uint32_t data)
{
    mmio_write32((uint32_t*)(lapic_base + reg), data);
}

void init_lapic(uint32_t base){
    lapic_base = base;

    lapic_write(LAPIC_TPR_REG, 0);
    lapic_write(LAPIC_DFR_REG, 0xFFFFFFFF);
    lapic_write(LAPIC_LDR_REG, 0x01000000);
    lapic_write(LAPIC_SPURIOUS_VECTOR_REG, 0x100 | 0xff);
}

uint8_t lapic_get_id(){
    return lapic_read(LAPIC_VERSION_REG) >> 24;
}

void lapic_send_eoi(){
    lapic_write(LAPIC_EOI_REG, 0);
}