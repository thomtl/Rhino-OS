#include <rhino/mm/mmio.h>

uint8_t mmio_read8(uint32_t* addr){
    vmm_map_page((void*)addr, (void*)addr, 0);
    volatile uint8_t* ptr = (uint8_t*)((uint32_t)addr);
    uint8_t val = *ptr;
    vmm_unmap_page((void*)addr);
    return val;
}

void mmio_write8(uint32_t* addr, uint8_t val){
    vmm_map_page((void*)addr, (void*)addr, 0);
    volatile uint8_t* ptr = (uint8_t*)((uint32_t)addr);
    *ptr = val;
    vmm_unmap_page((void*)addr);
}

uint16_t mmio_read16(uint32_t* addr){
    vmm_map_page((void*)addr, (void*)addr, 0);
    volatile uint16_t* ptr = (uint16_t*)((uint32_t)addr);
    uint16_t val = *ptr;
    vmm_unmap_page((void*)addr);
    return val;
}

void mmio_write16(uint32_t* addr, uint16_t val){
    vmm_map_page((void*)addr, (void*)addr, 0);
    volatile uint16_t* ptr = (uint16_t*)((uint32_t)addr);
    *ptr = val;
    vmm_unmap_page((void*)addr);
}

uint32_t mmio_read32(uint32_t* addr){
    vmm_map_page((void*)addr, (void*)addr, 0);
    volatile uint32_t* ptr = (uint32_t*)((uint32_t)addr);
    uint32_t val = *ptr;
    vmm_unmap_page((void*)addr);
    return val;
}

void mmio_write32(uint32_t* addr, uint32_t val){
    vmm_map_page((void*)addr, (void*)addr, 0);
    volatile uint32_t* ptr = (uint32_t*)((uint32_t)addr);
    *ptr = val;
    vmm_unmap_page((void*)addr);
}

uint64_t mmio_read64(uint32_t* addr){
    vmm_map_page((void*)addr, (void*)addr, 0);
    volatile uint64_t* ptr = (uint64_t*)((uint32_t)addr);
    uint64_t val = *ptr;
    vmm_unmap_page((void*)addr);
    return val;
}

void mmio_write64(uint32_t* addr, uint64_t val){
    vmm_map_page((void*)addr, (void*)addr, 0);
    volatile uint64_t* ptr = (uint64_t*)((uint32_t)addr);
    *ptr = val;
    vmm_unmap_page((void*)addr);
}