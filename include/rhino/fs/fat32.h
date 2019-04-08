#pragma once

#include <rhino/common.h>
#include <rhino/fs/vfs.h>
#include <libk/stdio.h>

struct fat32_bpb
{
    uint8_t jump_instruction[3];
    char OEM_name[8];
    uint16_t bytes_per_sector;
    uint8_t sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t number_of_fats;
    uint16_t root_entries;
    uint16_t total_sectors;
    uint8_t media_descriptor;
    uint16_t sectors_per_fat_old;
    uint16_t sectors_per_track;
    uint16_t heads;
    uint32_t hidden_sectors;
    uint32_t extended_sectors;
    uint32_t sectors_per_fat;
    uint16_t extended_flags;
    uint16_t file_system_version;
    uint32_t root_cluster_number;
    uint16_t fs_info_sector_number;
    uint16_t boot_sector_backup_sector;
    uint8_t reserved[12];
    uint8_t drive_number;
    uint8_t reserved1;
    uint8_t boot_signature;
    uint32_t serial_number;
    char label[11];
    char system_type[8];
} __attribute__((packed));

struct fat32_info_sector
{
    uint32_t signature1;
    uint8_t reserved[480];
    uint32_t signature2;
    uint32_t free_cluster_count;
    uint32_t next_free_cluster_count;
    uint8_t reserved1[12];
    uint32_t signature3;
} __attribute__((packed));

#define FAT32_INFO_SECTOR_SIG1 0x41615252
#define FAT32_INFO_SECTOR_SIG2 0x61417272
#define FAT32_INFO_SECTOR_SIG3 0xAA550000

#define FAT12_SYSTEM_TYPE "FAT12   "
#define FAT16_SYSTEM_TYPE "FAT16   "
#define FAT32_SYSTEM_TYPE "FAT32   "

#define FAT_MEDIA_DESCRIPTOR_FIXED_MEDIA 0xF8
#define FAT_MEDIA_DESCRIPTOR_FLOPPY_1_33M 0xF0
#define FAT_MEDIA_DESCRIPTOR_FLOPPY_1_20M 0xF9
#define FAT_MEDIA_DESCRIPTOR_FLOPPY_320K_AND_RAMDISK 0xFA
#define FAT_MEDIA_DESCRIPTOR_FLOPPY_640K 0xFB
#define FAT_MEDIA_DESCRIPTOR_FLOPPY_180K_AND_8INCH 0xFC
#define FAT_MEDIA_DESCRIPTOR_FLOPPY_360K 0xFD
#define FAT_MEDIA_DESCRIPTOR_FLOPPY_180K 0xFE
#define FAT_MEDIA_DESCRIPTOR_FLOPPY_320K 0xFF

struct fat32_directory_entry
{
    char file_name[8];
    char file_extension[3];
    uint8_t attribute;
    uint8_t reserved;
    uint8_t tenths_of_a_second;
    uint16_t creation_time;
    uint16_t creation_date;
    uint16_t date_last_accessed;
    uint16_t starting_cluster_high;
    uint16_t time;
    uint16_t date;
    uint16_t starting_cluster_low;
    uint32_t file_size;
} __attribute__((packed));

#define FAT_DIRECTORY_ENTRY_ATTRIBUTE_READ_ONLY 0
#define FAT_DIRECTORY_ENTRY_ATTRIBUTE_HIDDEN 1
#define FAT_DIRECTORY_ENTRY_ATTRIBUTE_SYSTEM 2
#define FAT_DIRECTORY_ENTRY_ATTRIBUTE_VOLUME_ID 3
#define FAT_DIRECTORY_ENTRY_ATTRIBUTE_DIRECTORY 4
#define FAT_DIRECTORY_ENTRY_ATTRIBUTE_ARCHIVE 5

void init_fat32(fs_node_t *file_node, uint64_t lba);

struct fat32_directory_entry fat32_get_entry_by_name(char *name);