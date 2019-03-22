#pragma once

#include <rhino/common.h>
#include <rhino/types/linked_list.h>


#define UDI_DRIVER_NAME_LENGTH 16

typedef struct {
    uint32_t width;
    uint32_t height;
    uint32_t bpp;
    bool allowRounding;
    bool clearFramebuffer;
    bool success;
} udi_video_mode_set_t;

typedef bool (*udi_graphics_set_video_mode)(udi_video_mode_set_t*);
typedef uint32_t (*udi_graphics_get_driver_id)(void);
typedef bool (*udi_graphics_enable_driver)(void);
typedef bool (*udi_graphics_disable_driver)(void);
typedef void (*udi_graphics_set_pixel)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t); // x,y,r,g,b
typedef void (*udi_graphics_draw_rect)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t); //x,y,w,h,r,g,b
typedef void* (*udi_graphics_get_framebuffer)();
typedef void (*udi_graphics_switch_banks)(uint32_t); // n

#define UDI_GRAPHICS_DRIVER_ID_PCI_VENDOR_ID 0
#define UDI_GRAPHICS_DRIVER_ID_LFB 16
#define UDI_GRAPHICS_DRIVER_ID_BANKING 17
#define UDI_GRAPHICS_DRIVER_ID_EXTENDED 18

typedef struct {
    char name[UDI_DRIVER_NAME_LENGTH];
    udi_graphics_set_video_mode set_video_mode;
    udi_graphics_get_driver_id get_driver_id;
    udi_graphics_enable_driver enable_driver;
    udi_graphics_enable_driver disable_driver;
    udi_graphics_set_pixel set_pixel;
    udi_graphics_draw_rect draw_rect;
    udi_graphics_get_framebuffer get_framebuffer;
    udi_graphics_switch_banks switch_banks;
} udi_graphics_driver_t;


typedef bool (*udi_disk_enable_driver)(void);
typedef bool (*udi_disk_disable_driver)(void);
typedef uint32_t (*udi_disk_read)(uint64_t, uint32_t, uint8_t*);
typedef uint32_t (*udi_disk_write)(uint64_t, uint32_t, uint8_t*);
typedef uint64_t (*udi_disk_get_size)(void);
typedef uint32_t (*udi_disk_get_block_size)(void);
typedef uint64_t (*udi_disk_get_amount_of_blocks)(void);
typedef uint32_t (*udi_disk_read_lba)(uint64_t, uint32_t, uint8_t*);
typedef uint32_t (*udi_disk_write_lba)(uint64_t, uint32_t, uint8_t*);

typedef struct {
    char name[UDI_DRIVER_NAME_LENGTH];
    udi_disk_enable_driver enable_driver;
    udi_disk_disable_driver disable_driver;
    udi_disk_read read;
    udi_disk_write write;
    udi_disk_get_size get_size;
    udi_disk_get_block_size get_block_size;
    udi_disk_get_amount_of_blocks get_amount_of_blocks;
    udi_disk_read_lba read_lba;
    udi_disk_write_lba write_lba;
} udi_disk_driver_t;

void udi_initialize();
void udi_register_graphics_driver(udi_graphics_driver_t* driver);
void udi_register_disk_driver(udi_disk_driver_t* driver);
udi_graphics_driver_t* udi_get_active_graphics_driver();
void udi_set_active_graphics_driver(udi_graphics_driver_t* driver);
udi_graphics_driver_t* udi_get_graphics_driver_by_name(char* name);
udi_disk_driver_t* udi_get_disk_driver_by_name(char* name);
udi_disk_driver_t* udi_get_disk_driver_by_index(uint32_t index);