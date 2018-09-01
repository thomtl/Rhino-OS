#pragma once

#include <rhino/common.h>
#include <rhino/arch/x86/ports.h>

#define PCI_IO_CMD 0xCF8
#define PCI_IO_DAT 0XCFC

uint16_t pci_config_read_word(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset);
uint16_t pci_check_vendor(uint8_t bus, uint8_t slot);
uint16_t pci_get_vendor_id(uint8_t bus, uint8_t device, uint8_t function);
uint8_t pci_get_header_type(uint8_t bus, uint8_t device, uint8_t function);
void pci_check_function(uint8_t bus, uint8_t device, uint8_t function);
void pci_check_device(uint8_t bus, uint8_t device);

void pci_check_bus(uint8_t bus);
