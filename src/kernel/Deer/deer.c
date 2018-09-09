#include <rhino/Deer/deer.h>
#include <libk/stdio.h>

size_t framebuffer;

void deer_clear_screen_col(uint8_t col){
    for(uint32_t i = 0; i < 720; i++){
        for(uint32_t j = 0; j < 480; j++) draw_pixel(i, j, col);
    }
}

void deer_start(){
    vga_set_320x200();
    framebuffer = (vga_get_fb_seg() + 0xC0000000);
    
    deer_clear_screen();

    deer_printf("abcdefghijklmnopqrstuvwxyz1234567890abcdefghijk\nlmnop");

    draw_rect(100, 100, 50, 50, 50);
}