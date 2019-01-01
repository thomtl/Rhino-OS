#include <rhino/arch/x86/pci.h>

uint8_t pci_get_programming_interface(uint8_t bus, uint8_t device, uint8_t function){
  uint16_t t = pci_config_read(bus, device, function, 9, PCI_SIZE_BYTE);
  return t;
}

uint8_t pci_get_class(uint8_t bus, uint8_t device, uint8_t function){
  uint16_t t = pci_config_read(bus, device, function, 11, PCI_SIZE_BYTE);
  return t;
}

uint8_t pci_get_subclass(uint8_t bus, uint8_t device, uint8_t function){
  uint16_t t = pci_config_read(bus, device, function, 10, PCI_SIZE_BYTE);
  return t;
}

uint8_t pci_get_secondary_bus(uint8_t bus, uint8_t device, uint8_t function){
  uint16_t t = pci_config_read(bus, device, function, 19, PCI_SIZE_BYTE);
  return t;
}

uint16_t pci_get_device_id(uint8_t bus, uint8_t device, uint8_t function){
  uint16_t t = pci_config_read(bus, device, function, 2, PCI_SIZE_WORD);
  return t;
}

uint16_t pci_get_vendor_id(uint8_t bus, uint8_t device, uint8_t function){
  uint16_t t = pci_config_read(bus, device, function, 0, PCI_SIZE_WORD);
  return t;
}

uint8_t pci_get_header_type(uint8_t bus, uint8_t device, uint8_t function){
  uint16_t t = pci_config_read(bus, device, function, 0x0E, PCI_SIZE_BYTE);
  return t;
}

uint32_t pci_config_read(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset, uint8_t len){
  uint32_t ret;
  uint32_t addr = 0x80000000 | (bus << 16) | (slot << 11) | (function << 8) | (offset & 0xFC);
  outd(PCI_IO_CMD, addr);
  ret = (ind(PCI_IO_DAT) >> ((offset & 3) * 8)) & (0xFFFFFFFF >> ((4 - len) * 8));
  return ret;
}

uint32_t pci_config_write(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset, uint8_t len, uint32_t val){
  uint32_t ret;
  uint32_t addr = 0x80000000 | (bus << 16) | (slot << 11) | (function << 8) | (offset & 0xFC);
  outd(PCI_IO_CMD, addr);
  ret = (ind(PCI_IO_DAT) >> ((offset & 3) * 8)) & (0xFFFFFFFF >> ((4 - len) * 8));

  outd(PCI_IO_CMD, addr);
  if(len == PCI_SIZE_BYTE) outb(PCI_IO_DAT, val);
  else if(len == PCI_SIZE_WORD) outw(PCI_IO_DAT, val);
  else if(len == PCI_SIZE_DWORD) outd(PCI_IO_DAT, val);
  else debug_log("[PCI]: Unknown len\n");
  return ret;
}

void pci_print_device_info(uint8_t bus, uint8_t device, uint8_t function){
  char buf[25] = "";
  kprint("Class: ");
  hex_to_ascii(pci_get_class(bus, device, function), buf);
  kprint(buf);
  for(int i = 0; i < 25; i++) buf[i] = '\0';

  kprint(", Subclass: ");
  hex_to_ascii(pci_get_subclass(bus, device, function), buf);
  kprint(buf);
  for(int i = 0; i < 25; i++) buf[i] = '\0';

  kprint(", VendorID: ");
  hex_to_ascii(pci_get_vendor_id(bus, device, function), buf);
  kprint(buf);

  for(int i = 0; i < 25; i++) buf[i] = '\0';

  kprint(", DeviceID: ");
  hex_to_ascii(pci_get_device_id(bus, device, function), buf);
  kprint(buf);

  for(int i = 0; i < 25; i++) buf[i] = '\0';

  kprint(", ");
  hex_to_ascii(bus, buf);
  kprint(buf);

  for(int i = 0; i < 25; i++) buf[i] = '\0';

  kprint(":");
  hex_to_ascii(device, buf);
  kprint(buf);
  for(int i = 0; i < 25; i++) buf[i] = '\0';

  kprint(":");
  hex_to_ascii(function, buf);
  kprint(buf);

  switch(pci_get_class(bus, device, function)){
      case PCI_UNCLASSIFIED:
        kprint(" Unclassified");
        break;
      case PCI_STORAGE_CONTROLLER:
        kprint(" Storage Controller");
        break;
      case PCI_NETWORK_CONTROLLER:
        kprint(" Network Controller");
        break;
      case PCI_DISPLAY_CONTROLLER:
        kprint(" Display Controller");
        break;
      case PCI_MULTIMEDIA_CONTROLLER:
        kprint(" Multimedia Controller");
        break;
      case PCI_MEMORY_CONTROLLER:
        kprint(" Memory Controller");
        break;
      case PCI_BRIDGE_CONTROLLER:
        kprint(" Bridge Device");
        break;
      default:
        kprint("Unkown Device");
        break;
  }

  kprint("\n");
}


void pci_check_function(uint8_t bus, uint8_t device, uint8_t function){
  uint8_t baseClass, subClass, headerType;
  uint16_t vendorID, deviceID;
  vendorID = pci_get_vendor_id(bus, device, function);
  if(vendorID == PCI_VENDOR_UNUSED) return;
  baseClass = pci_get_class(bus, device, function);
  subClass = pci_get_subclass(bus, device, function);
  deviceID = pci_get_device_id(bus, device, function);

  if((vendorID == 0x1234 && deviceID == 0x1111) || (vendorID == 0x80EE && deviceID == 0xBEEF) || (vendorID == 0x10de && deviceID == 0x0a20)) init_bga(bus, device, function);
  if(baseClass == 0x1 && (subClass == 0x1 || subClass == 0x5)) ata_init(bus,  device, function);

  baseClass = pci_get_class(bus, device, function);
  subClass = pci_get_subclass(bus, device, function);
  headerType = pci_get_header_type(bus, device, function);
  if( (baseClass == PCI_CLASS_BRIDGE) && (subClass == PCI_SUBCLASS_BRIDGE_PCI_TO_PCI) && (headerType == 0x01)){
    pci_check_bus(pci_get_secondary_bus(bus, device, function));
  }

  //pci_print_device_info(bus, device, function);
}

void pci_check_device(uint8_t bus, uint8_t device){
  uint8_t function = 0;
  uint16_t vendorID = pci_get_vendor_id(bus, device, function);
  if(vendorID == PCI_VENDOR_UNUSED) return;
  //pci_check_function(bus, device, function);
  uint8_t headerType = pci_get_header_type(bus, device, function);
  if(BIT_IS_SET(headerType, 7)){
    for(function = 0; function < PCI_FUNCTION_N; function++){
      pci_check_function(bus, device, function);
    }
  }
}

void pci_check_bus(uint8_t bus){
  for(uint16_t dev = 0; dev < PCI_DEVICE_N; dev++){
    pci_check_device(bus, dev);
  }
}

void pci_check_all_buses(void){
  debug_log("[PCI]: Enumerating PCI\n");
  uint8_t function = 0;
  uint8_t headerType;

  headerType = pci_get_header_type(0, 0, function);
  if(!BIT_IS_SET(headerType, 7)){
    pci_check_bus(0);
  } else {
    for(function = 0; function < PCI_FUNCTION_N; function++){
      if(pci_get_vendor_id(0, 0, function) == PCI_VENDOR_UNUSED) break;
      pci_check_bus(function);
    }
  }
  debug_log("[PCI]: PCI Enumerated\n");
}

uint32_t pci_read_bar(uint8_t bus, uint8_t device, uint8_t function, uint8_t bar){
  return pci_config_read(bus, device, function, bar, PCI_SIZE_DWORD);
}