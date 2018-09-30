#include <rhino/Deer/deergui.h>

extern size_t framebuffer;

void draw_pixel(size_t x, size_t y, uint8_t r, uint8_t g, uint8_t b)
{
	*(uint8_t*)(&((uint8_t *)framebuffer)[(y * (SCREEN_WIDTH) + x) * 3]) = b;
    *(uint8_t*)(&((uint8_t *)framebuffer)[(y * (SCREEN_WIDTH) + x) * 3 + 1]) = g;
    *(uint8_t*)(&((uint8_t *)framebuffer)[(y * (SCREEN_WIDTH) + x) * 3 + 2]) = r;
}

void draw_rect(size_t x, size_t y, size_t w, size_t h, uint8_t r, uint8_t g, uint8_t b){
    for(size_t i = 0; i < w; i++){
        for(size_t j = 0; j < h; j++){
           	*(uint8_t*)(&((uint8_t *)framebuffer)[((y + j) * (SCREEN_WIDTH) + (x + i)) * 3]) = b;
            *(uint8_t*)(&((uint8_t *)framebuffer)[((y + j) * (SCREEN_WIDTH) + (x + i)) * 3 + 1]) = g;
            *(uint8_t*)(&((uint8_t *)framebuffer)[((y + j) * (SCREEN_WIDTH) + (x + i)) * 3 + 2]) = r;
        }
    }
}

void draw_circle(int32_t x0, int32_t y0, int32_t radius, uint8_t r, uint8_t g, uint8_t b)
{
    int32_t x = radius-1;
    int32_t y = 0;
    int32_t dx = 1;
    int32_t dy = 1;
    int32_t err = dx - (radius << 1);

    while (x >= y)
    {
        draw_pixel(x0 + x, y0 + y, r, g, b);
        draw_pixel(x0 + y, y0 + x, r, g, b);
        draw_pixel(x0 - y, y0 + x, r, g, b);
        draw_pixel(x0 - x, y0 + y, r, g, b);
        draw_pixel(x0 - x, y0 - y, r, g, b);
        draw_pixel(x0 - y, y0 - x, r, g, b);
        draw_pixel(x0 + y, y0 - x, r, g, b);
        draw_pixel(x0 + x, y0 - y, r, g, b);

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