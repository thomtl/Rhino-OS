#include <rhino/arch/x86/pci.h>

#include <rhino/arch/x86/drivers/screen.h>
#include <libk/string.h>

uint16_t pci_config_read_word(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset){
  uint32_t address;
  uint32_t lbus = (uint32_t)bus;
  uint32_t lslot = (uint32_t)slot;
  uint32_t lfunc = (uint32_t)function;
  uint16_t tmp = 0;

  address = (uint32_t)((lbus << 16) | (lslot << 11) | (lfunc << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));

  outd(PCI_IO_CMD, address);
  tmp = (uint16_t)((inb(PCI_IO_DAT) >> ((offset & 2) * 8)) & 0xFFFF);
  return tmp;
}

uint16_t pci_check_vendor(uint8_t bus, uint8_t slot){
  uint16_t vendor, device;
  if((vendor = pci_config_read_word(bus, slot,0,0)) != 0xFFFF){
    device = pci_config_read_word(bus, slot, 0, 2);
    //if(device != 0){
      char buf[25] = "";
      hex_to_ascii(device, buf);
      kprint(buf);
      kprint(" : ");
      for(int i = 0; i < 25; i++) buf[i] = '\0';

      hex_to_ascii(pci_config_read_word(bus, slot, 0, 11), buf);
      kprint(buf);
      kprint("\n");
      for(int i = 0; i < 25; i++) buf[i] = '\0';
    //}
  }
  return device;
}

uint16_t pci_get_vendor_id(uint8_t bus, uint8_t device, uint8_t function){
  /*uint32_t tmp;
  uint_t tmp1, tmp2;
  tmp1 = pci_config_read_word(bus, device, function, 0);
  tmp2 = pci_config_read_word(bus, device, function, 1);

  tmp = (tmp1) | (tmp2 << 8);*/
  return pci_config_read_word(bus, device, function, 0);
}

uint8_t pci_get_header_type(uint8_t bus, uint8_t device, uint8_t function){
  return (pci_config_read_word(bus, device, function, 0xE0) & 0xFF);
}

void pci_check_function(uint8_t bus, uint8_t device, uint8_t function){
  char buf[25];
  int_to_ascii(pci_config_read_word(bus, device, function, 10), buf);
  kprint(buf);
  kprint("\n");
}

void pci_check_device(uint8_t bus, uint8_t device){
  uint8_t function = 0;
  uint16_t vendorID = pci_get_vendor_id(bus, device, function);
  if(vendorID == 0xFFFF){
    kprint("N");
     return;
  }
  pci_check_function(bus, device, function);
  uint8_t headerType = pci_get_header_type(bus, device, function);
  if((headerType & 0x80) != 0){
    for(function = 1; function < 8; function++){
      pci_check_function(bus, device, function);
      return;
    }
  }
  //kprint("single function device\n");

}

void pci_check_bus(uint8_t bus){
  for(uint16_t dev = 0; dev < 32; dev++){
    pci_check_device(bus, dev);
  }
}
