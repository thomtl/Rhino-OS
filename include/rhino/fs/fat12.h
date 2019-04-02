#pragma once
#include <rhino/common.h>
#include <rhino/fs/vfs.h>

struct fat12_bpb {
    uint8_t jump_instruction[3];
    char OEM_name[8];
    uint16_t bytes_per_sector;
    uint8_t sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t number_of_fats;
    uint16_t root_entries;
    uint16_t total_sectors;
    uint8_t media_descriptor;
    uint16_t sectors_per_fat;
    uint16_t sectors_per_track;
    uint16_t heads;
    uint32_t hidden_sectors;
    uint32_t extended_sectors;
    uint8_t drive_number;
    uint8_t reserved;
    uint8_t boot_signature;
    uint32_t serial_number;
    char label[11];
    char system_type[8];
} __attribute__((packed));

#define FAT12_SYSTEM_TYPE "FAT12   "
#define FAT16_SYSTEM_TYPE "FAT16   "

#define FAT_MEDIA_DESCRIPTOR_FIXED_MEDIA 0xF8
#define FAT_MEDIA_DESCRIPTOR_FLOPPY_1_33M 0xF0
#define FAT_MEDIA_DESCRIPTOR_FLOPPY_1_20M 0xF9
#define FAT_MEDIA_DESCRIPTOR_FLOPPY_320K_AND_RAMDISK 0xFA
#define FAT_MEDIA_DESCRIPTOR_FLOPPY_640K 0xFB
#define FAT_MEDIA_DESCRIPTOR_FLOPPY_180K_AND_8INCH 0xFC
#define FAT_MEDIA_DESCRIPTOR_FLOPPY_360K 0xFD
#define FAT_MEDIA_DESCRIPTOR_FLOPPY_180K 0xFE
#define FAT_MEDIA_DESCRIPTOR_FLOPPY_320K 0xFF

struct fat12_directory_entry {
    char file_name[8];
    char file_extension[3];
    uint8_t attribute;
    char reserved[10];
    uint16_t time;
    uint16_t date;
    uint16_t starting_cluster;
    uint32_t file_size;
} __attribute__((packed));