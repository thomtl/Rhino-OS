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

// Midpoint32_t Circle Algorithm
void draw_circle(int32_t x0, int32_t y0, int32_t radius, uint8_t col)
{
    int32_t x = radius-1;
    int32_t y = 0;
    int32_t dx = 1;
    int32_t dy = 1;
    int32_t err = dx - (radius << 1);

    while (x >= y)
    {
        draw_pixel(x0 + x, y0 + y, col);
        draw_pixel(x0 + y, y0 + x, col);
        draw_pixel(x0 - y, y0 + x, col);
        draw_pixel(x0 - x, y0 + y, col);
        draw_pixel(x0 - x, y0 - y, col);
        draw_pixel(x0 - y, y0 - x, col);
        draw_pixel(x0 + y, y0 - x, col);
        draw_pixel(x0 + x, y0 - y, col);

        if (err <= 0)
        {
            y++;
            err += dy;
            dy += 2;
        }
        
        if (err > 0)
        {
            x--;
            dx += 2;
            err += dx - (radius << 1);
        }
    }
}

void deer_start(){
    vga_set_320x200();
    fb = (vga_get_fb_seg() + 0xC0000000);
    
    for(uint32_t i = 0; i < 320; i++){
        for(uint32_t j = 0; j < 200; j++) draw_pixel(i, j, 0);
    }

    draw_circle(100, 100, 50, 255);

    uint32_t x = 100, y = 100;
    while(1){
        char c = getchar();
        if(c == 'w') y--;
        if(c == 'a') x--;
        if(c == 's') y++;
        if(c == 'd') x++;
            for(uint32_t i = 0; i < 320; i++)
                for(uint32_t j = 0; j < 200; j++) 
                    draw_pixel(i, j, 0);
        draw_circle(x, y, 50, 255);
    }
}