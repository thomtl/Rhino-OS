#pragma once

#include <rhino/common.h>
#include <rhino/arch/x86/ports.h>

#define PCI_IO_CMD 0xCF8
#define PCI_IO_DAT 0XCFC

#define PCI_VENDOR_UNUSED 0xFFFF

#define PCI_MULTIFUNCTION_MASK 0x80

#define PCI_BUS_N 256
#define PCI_DEVICE_N 32
#define PCI_FUNCTION_N 8
uint16_t pci_config_read_word(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset);
void pci_check_function(uint8_t bus, uint8_t device, uint8_t function);
void pci_check_device(uint8_t bus, uint8_t device);
void pci_check_bus(uint8_t bus);
void pci_check_all_buses(void);
