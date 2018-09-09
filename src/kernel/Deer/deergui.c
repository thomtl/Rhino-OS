#include <rhino/Deer/deergui.h>

extern size_t framebuffer;

void draw_pixel(size_t x, size_t y, uint8_t c)
{
	uint8_t* addr = (uint8_t*)(framebuffer + ((SCREEN_WIDTH * (y)) + (x)));
    *addr = c;
}

void draw_rect(size_t x, size_t y, size_t w, size_t h, uint8_t c){
    for(size_t i = 0; i < w; i++){
        for(size_t j = 0; j < h; j++){
	        uint8_t* addr = (uint8_t*)(framebuffer + ((SCREEN_WIDTH * (j + y)) + (i + x)));
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