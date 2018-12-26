#pragma once
#include <rhino/common.h>

typedef struct {
    uint8_t drive_attributes;
    uint8_t cylinder_address_partition_start;
    uint8_t head_address_partition_start;
    uint8_t sector_address_partition_start;
    uint8_t partition_type;
    uint8_t cylinder_address_partition_end;
    uint8_t head_address_partition_end;
    uint8_t sector_address_partition_end;
    uint32_t lba_address_partition_start;
    uint32_t number_of_sectors;
} __attribute__((packed)) mbr_entry_t;

typedef struct {
    uint8_t boot_code[436];
    uint8_t disk_id[10];
    mbr_entry_t first_partition_entry;
    mbr_entry_t second_partition_entry;
    mbr_entry_t third_partition_entry;
    mbr_entry_t fourth_partition_entry;
    uint16_t boot_signature;
} __attribute__((packed)) mbr_t;