#pragma once

#include <rhino/arch/x86/ports.h>
#include <libk/string.h>
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 200

void vga_set_320x200();
unsigned vga_get_fb_seg(void);