#include <rhino/Deer/deer.h>
#include <libk/stdio.h>
size_t fb;

void draw_pixel(size_t x, size_t y, uint8_t c)
{
	uint8_t* addr = (uint8_t*)(fb + ((SCREEN_WIDTH * (y)) + (x)));
    *addr = c;
}

void draw_rect(size_t x, size_t y, size_t w, size_t h, uint8_t c){
    for(size_t i = 0; i < w; i++){
        for(size_t j = 0; j < h; j++){
	        uint8_t* addr = (uint8_t*)(fb + ((SCREEN_WIDTH * (j + y)) + (i + x)));
            *addr = c;
        }
    }
}

void deer_start(){
    vga_set_320x200();
    fb = (vga_get_fb_seg() + 0xC0000000);
    
    for(uint32_t i = 0; i < 320; i++){
        for(uint32_t j = 0; j < 200; j++) draw_pixel(i, j, 0);
    }

    draw_rect(50, 50, 100, 100, 50);

    int x = 50, y = 50;
    while(1){
        char c = getchar();
        if(c == 'w') y--;
        if(c == 'a') x--;
        if(c == 's') y++;
        if(c == 'd') x++;
            for(uint32_t i = 0; i < 320; i++)
                for(uint32_t j = 0; j < 200; j++) 
                    draw_pixel(i, j, 0);
        draw_rect(x, y, 100, 100, 50);
    }
}