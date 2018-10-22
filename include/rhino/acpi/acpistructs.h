#pragma once
#include <rhino/common.h>

typedef struct {
 char Signature[8];
 uint8_t Checksum;
 char OEMID[6];
 uint8_t Revision;
 uint32_t RsdtAddress;
} __attribute__ ((packed)) RSDP;

typedef struct {
 RSDP firstPart;
 
 uint32_t Length;
 uint64_t XsdtAddress;
 uint8_t ExtendedChecksum;
 uint8_t reserved[3];
} __attribute__ ((packed)) XSDP;

typedef struct {
  char Signature[4];
  uint32_t Length;
  uint8_t Revision;
  uint8_t Checksum;
  char OEMID[6];
  char OEMTableID[8];
  uint32_t OEMRevision;
  uint32_t CreatorID;
  uint32_t CreatorRevision;
} __attribute__((packed)) SDTHeader;

typedef struct {
  SDTHeader h;
  //uint32_t PointerToOtherSDT[(h.Length - sizeof(h)) / 4];
  // Needs to be above just can't use it because i can't access uninitalized values
  uint32_t PointerToOtherSDT[25]; // 25 because 25
} __attribute__((packed)) RSDT;

typedef struct {
  SDTHeader h;
  //uint64_t PointerToOtherSDT[(h.Length - sizeof(h)) / 8];
  // Needs to be above just can't use it because i can't access uninitalized values
  uint64_t PointerToOtherSDT[25];
} __attribute__((packed)) XSDT;

#define PHI_ACPI_ADDRESS_SPACE_SYSTEM_MEM 0
#define PHI_ACPI_ADDRESS_SPACE_SYSTEM_IO 1
#define PHI_ACPI_ADDRESS_SPACE_PCI_CONFIG_SPACE 2
#define PHI_ACPI_ADDRESS_SPACE_EMBEDDED_CONTROLLER 3
#define PHI_ACPI_ADDRESS_SPACE_SMBUS 4
#define PHI_ACPI_ADDRESS_SPACE_FUNCTION_FIXED_HARDWARE 0x7F

#define PHI_ACPI_ACCESS_SIZE_UNDEFINED 0
#define PHI_ACPI_ACCESS_SIZE_BYTE 1
#define PHI_ACPI_ACCESS_SIZE_WORD 2
#define PHI_ACPI_ACCESS_SIZE_DWORD 3
#define PHI_ACPI_ACCESS_SIZE_QWORD 4

typedef struct
{
  uint8_t AddressSpace;
  uint8_t BitWidth;
  uint8_t BitOffset;
  uint8_t AccessSize;
  uint64_t Address;
} __attribute__((packed)) GenericAddressStructure;

typedef struct 
{
    SDTHeader h;
    uint32_t FirmwareCtrl;
    uint32_t Dsdt;
 
    // field used in ACPI 1.0; no longer in use, for compatibility only
    uint8_t  Reserved;
 
    uint8_t  PreferredPowerManagementProfile;
    uint16_t SCI_Interrupt;
    uint32_t SMI_CommandPort;
    uint8_t  AcpiEnable;
    uint8_t  AcpiDisable;
    uint8_t  S4BIOS_REQ;
    uint8_t  PSTATE_Control;
    uint32_t PM1aEventBlock;
    uint32_t PM1bEventBlock;
    uint32_t PM1aControlBlock;
    uint32_t PM1bControlBlock;
    uint32_t PM2ControlBlock;
    uint32_t PMTimerBlock;
    uint32_t GPE0Block;
    uint32_t GPE1Block;
    uint8_t  PM1EventLength;
    uint8_t  PM1ControlLength;
    uint8_t  PM2ControlLength;
    uint8_t  PMTimerLength;
    uint8_t  GPE0Length;
    uint8_t  GPE1Length;
    uint8_t  GPE1Base;
    uint8_t  CStateControl;
    uint16_t WorstC2Latency;
    uint16_t WorstC3Latency;
    uint16_t FlushSize;
    uint16_t FlushStride;
    uint8_t  DutyOffset;
    uint8_t  DutyWidth;
    uint8_t  DayAlarm;
    uint8_t  MonthAlarm;
    uint8_t  Century;
 
    // reserved in ACPI 1.0; used since ACPI 2.0+
    uint16_t BootArchitectureFlags;
 
    uint8_t  Reserved2;
    uint32_t Flags;
 
    // 12 byte structure; see below for details
    GenericAddressStructure ResetReg;
 
    uint8_t  ResetValue;
    uint8_t  Reserved3[3];
 
    // 64bit pointers - Available on ACPI 2.0+
    uint64_t                X_FirmwareControl;
    uint64_t                X_Dsdt;
 
    GenericAddressStructure X_PM1aEventBlock;
    GenericAddressStructure X_PM1bEventBlock;
    GenericAddressStructure X_PM1aControlBlock;
    GenericAddressStructure X_PM1bControlBlock;
    GenericAddressStructure X_PM2ControlBlock;
    GenericAddressStructure X_PMTimerBlock;
    GenericAddressStructure X_GPE0Block;
    GenericAddressStructure X_GPE1Block;
} __attribute__((packed)) FADT;

typedef struct
{
    SDTHeader h;
    uint8_t hardware_rev_id;
    uint8_t comparator_count:5;
    uint8_t counter_size:1;
    uint8_t reserved:1;
    uint8_t legacy_replacement:1;
    uint16_t pci_vendor_id;
    GenericAddressStructure address;
    uint8_t hpet_number;
    uint16_t minimum_tick;
    uint8_t page_protection;
} __attribute__((packed)) HPET;

// After the madt_flags there is the interrupt controller structure it should be parsed seperatly
typedef struct {
  SDTHeader h;
  uint32_t lapic_addr;
  uint32_t madt_flags;
} __attribute__ ((packed)) MADT;

#define APCI_MADT_FLAGS_PCAT_COMPAT 0