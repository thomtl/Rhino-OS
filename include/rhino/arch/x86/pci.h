#pragma once

#include <rhino/common.h>
#include <rhino/arch/x86/io.h>
#include <rhino/arch/x86/drivers/bga.h>
#include <rhino/arch/x86/drivers/ata.h>
#include <rhino/arch/x86/drivers/screen.h>
#include <libk/string.h>


#define PCI_IO_CMD 0xCF8
#define PCI_IO_DAT 0XCFC

#define PCI_VENDOR_UNUSED 0xFFFF

#define PCI_MULTIFUNCTION_MASK 0x80

#define PCI_BUS_N 256
#define PCI_DEVICE_N 32
#define PCI_FUNCTION_N 8

#define PCI_CLASS_BRIDGE 0x06
#define PCI_SUBCLASS_BRIDGE_PCI_TO_PCI 0x04

#define PCI_BAR_0 0x10
#define PCI_BAR_1 0x14
#define PCI_BAR_2 0x18
#define PCI_BAR_3 0x1C
#define PCI_BAR_4 0x20
#define PCI_BAR_5 0x24

#define PCI_UNCLASSIFIED 0x0
#define PCI_STORAGE_CONTROLLER 0x1
#define PCI_NETWORK_CONTROLLER 0x2
#define PCI_DISPLAY_CONTROLLER 0x3
#define PCI_MULTIMEDIA_CONTROLLER 0x4
#define PCI_MEMORY_CONTROLLER 0x5
#define PCI_BRIDGE_CONTROLLER 0x6

#define PCI_SIZE_BYTE 1
#define PCI_SIZE_WORD 2
#define PCI_SIZE_DWORD 4
#define PCI_SIZE_QWORD 9

uint32_t pci_config_read(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset, uint8_t len);
uint32_t pci_config_write(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset, uint8_t len, uint32_t val);
void pci_check_function(uint8_t bus, uint8_t device, uint8_t function);
void pci_check_device(uint8_t bus, uint8_t device);
void pci_check_bus(uint8_t bus);
void pci_check_all_buses(void);
uint32_t pci_read_bar(uint8_t bus, uint8_t device, uint8_t function, uint8_t bar);

uint8_t pci_get_class(uint8_t bus, uint8_t device, uint8_t function);
uint8_t pci_get_subclass(uint8_t bus, uint8_t device, uint8_t function);
uint8_t pci_get_subclass(uint8_t bus, uint8_t device, uint8_t function);
uint8_t pci_get_secondary_bus(uint8_t bus, uint8_t device, uint8_t function);
uint16_t pci_get_device_id(uint8_t bus, uint8_t device, uint8_t function);
uint16_t pci_get_vendor_id(uint8_t bus, uint8_t device, uint8_t function);
uint8_t pci_get_header_type(uint8_t bus, uint8_t device, uint8_t function);