// x86 IOAPIC Driver
#include <rhino/common.h>
#include <rhino/mm/vmm.h>
#include <rhino/acpi/acpi.h>
#include <rhino/mm/mmio.h>

#define IOREGSEL 0x0
#define IOWIN 0x10

#define IOAPICIO 0x0
#define IOAPICVER 0x1
#define IOAPICARB 0x2
#define IOREDTBL 0x10

#define IRQ_BASE                        0x20

#define IRQ_TIMER                       0x00
#define IRQ_KEYBOARD                    0x01
#define IRQ_COM2                        0x03
#define IRQ_COM1                        0x04
#define IRQ_FLOPPY                      0x06
#define IRQ_ATA0                        0x0e
#define IRQ_ATA1                        0x0f

#define INT_TIMER                       0x20
#define INT_SPURIOUS                    0xff

void init_ioapic(uint32_t base);
void ioapic_set_entry(uint8_t index, uint64_t data);